#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/addons-module.h"
#include "ns3/netanim-module.h"
#include <chrono>
#include <random>

/*

    --------------------------------------------
    Code developed by Agnaldo Batista <agnaldosb@gmail.com> and João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>,, April 2024
    --------------------------------------------

  To run this simulation, open a terminal in ns3's root folder (ns3-3.34/) and run a command like:

    /waf --run "simulation --numUAV=40"
  
  Where "numUAV" is the desired number of UAVs in the simulation.
  
  BE CAREFUL! 
	- There must exist a "Logs" folder inside ns3's root folder (ns3-3.34/), otherwise Logs will not be saved!
    - Maximum nodes is 40
    - Simulation runtime is defined at line 214

*/

using namespace ns3;


int main (int argc, char *argv[]) {
  
  //-----------------------
  // Create traces Folder
  //-----------------------

  Statistics statistics;
  time_t day = time(NULL);
  struct tm time = *localtime(&day); 
  std::stringstream location;
  location << "./Logs/" << time.tm_mday << '_' << time.tm_mon+1 << '_'  << time.tm_year + 1900 << "_at_" << time.tm_hour << time.tm_min;
  mkdir(location.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  statistics.filepath = location.str();

  //-----------------------
  // Check number of nodes
  //-----------------------

   // Creating and defining seeds to be used

  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  //seed = 1905136375;
  srand(seed);

  uint32_t numUAV = 2;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("numUAV", "Number of UAVs", numUAV);

  cmd.Parse (argc,argv);

  if (numUAV > 41) {
    std::cout << "Numero maximo de UAVs excedido. Tente novamente com um numero menor." << std::endl;
    return 1;
  } else if (numUAV < 2)
  {
    std::cout << "Numero minimo de UAVs nao alcancado. Tente novamente com um numero maior." << std::endl;
    return 1;
  }

  std::stringstream text;
  text << std::endl << "The seed is: " << seed << std::endl << std::endl;
  text << "Drones:" << std::endl;
  text << "Number of drones: " << numUAV << std::endl;
  statistics.addSimulationInfo(text.str());

  //-----------------------
  // Mobility
  //-----------------------

  MobilityHelper mobilityUAVs;

  mobilityUAVs.SetMobilityModel ( "ns3::RandomWalk2dMobilityModel",
                                  "Mode", StringValue ("Time"),
                                  "Time", StringValue ("1s"),
                                  "Speed", StringValue ("ns3::UniformRandomVariable[Min=20|Max=20]"),
                                  "Bounds", StringValue ("0|1500|0|1500"));
  mobilityUAVs.SetPositionAllocator("ns3::RandomBoxPositionAllocator",
                                    "X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1500.0]"),
                                    "Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1500.0]"),
                                    "Z", StringValue ("ns3::UniformRandomVariable[Min=91.4|Max=91.4]"));

  // Create nodes to be used during simulation

  NodeContainer Nodes;
  Nodes.Create(numUAV);
  mobilityUAVs.Install(Nodes);

  statistics.addSimulationInfo("Mobility:");
  statistics.addSimulationInfo("MobilityModel is RandomWalk2D");
  statistics.addSimulationInfo("Box size is 1500");
  statistics.addSimulationInfo("Altitude of nodes is 91.4m\n");

  //-----------------------
  // Wifi Config
  //-----------------------

  WifiHelper wifi;
  
  wifi.SetStandard (WIFI_STANDARD_80211n_2_4GHZ);
  Config::SetDefault ("ns3::LogDistancePropagationLossModel::ReferenceLoss", DoubleValue (40.046));
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", StringValue ("HtMcs3"),
                                "ControlMode", StringValue ("HtMcs3"));

  // MAC Layer non QoS
  WifiMacHelper wifiMac;
  wifiMac.SetType("ns3::AdhocWifiMac");

  // PHY layer
  YansWifiPhyHelper wifiPhy;
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
  wifiPhy.SetChannel(wifiChannel.Create());

  // Creating and installing netdevices in all nodes
  NetDeviceContainer devices;
  devices = wifi.Install(wifiPhy, wifiMac, Nodes);

  // Create and install Internet stack protocol
  InternetStackHelper stack;
  stack.Install(Nodes);

  // Set IPv4 address to node's interfaces
  Ipv4AddressHelper address;
  Ipv4InterfaceContainer NodesInterface;
  address.SetBase("1.1.1.0", "255.255.255.0");

  NodesInterface = address.Assign(devices);

  //-----------------------
  // Applications Config
  //-----------------------

  uint16_t port = 9;
  double start = 0;
  double stop = 1195;
  NodeContainer::Iterator it;
  Address SinkBroadAddress(InetSocketAddress(Ipv4Address::GetAny(), port)); // SinkAddress for messages
  
  for (it = Nodes.Begin(); it != Nodes.End(); it++) {
    Ptr<MyPktSink> SinkApp = CreateObject<MyPktSink>();
    (*it)->AddApplication(SinkApp);
    SinkApp->SetStartTime(Seconds(start));
    SinkApp->SetStopTime(Seconds(stop));
    SinkApp->Setup(SinkBroadAddress, 1, seed); // 1 -> UDP, 2 -> TCP
  }

  for (it = Nodes.Begin(); it != Nodes.End(); it++) {

    Ptr<MyOnOff> OnOffApp = CreateObject<MyOnOff>(); 
    (*it)->AddApplication(OnOffApp); 

    // Set to send to broadcast address
    OnOffApp->Setup(InetSocketAddress(Ipv4Address("255.255.255.255"), 9), 1, seed); // 1 -> UDP, 2 -> TCP
    OnOffApp->SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=0.1]"));
    OnOffApp->SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.9]"));
    OnOffApp->SetAttribute("DataRate", StringValue("500kb/s"));
    OnOffApp->SetAttribute("PacketSize", UintegerValue(6));

    OnOffApp->SetStartTime(Seconds(start));
    OnOffApp->SetStopTime(Seconds(stop));
    start += 0.08; // Avoid to start all the OnOff together
  }

  //-----------------------
  // Traces Config
  //-----------------------

  // Callback Trace to Collect data from MyPktSink Application
  
  for (it = Nodes.Begin(); it != Nodes.End(); it++) {
    uint32_t nodeID = (*it)->GetId();
    std::ostringstream paramTest;
    std::ostringstream paramTest2;
    paramTest << "/NodeList/" << (nodeID) << "/ApplicationList/*/$ns3::MyPktSink/RxTraces";
    paramTest2 << "/NodeList/" << (nodeID) << "/ApplicationList/*/$ns3::MyPktSink/TxTraces";
    Config::ConnectWithoutContext(paramTest.str().c_str(), MakeCallback(&Statistics::receiverCallback, &statistics));
    Config::ConnectWithoutContext(paramTest2.str().c_str(), MakeCallback(&Statistics::senderCallback, &statistics));
  }

  // Callback Trace to Collect data from MyOnOff Application
  
  for (it = Nodes.Begin(); it != Nodes.End(); it++) {
    uint32_t nodeID = (*it)->GetId();
    std::ostringstream paramTest;
    paramTest << "/NodeList/" << (nodeID) << "/ApplicationList/*/$ns3::MyOnOff/TxTraces";
    Config::ConnectWithoutContext(paramTest.str().c_str(), MakeCallback(&Statistics::senderCallback, &statistics));
  }

  
  statistics.addSimulationInfo(text.str());


  Simulator::Stop(Seconds (1200));
  statistics.addSimulationInfo("\nSimulation runtime is 1200s");
  Simulator::Run(); 
  Simulator::Destroy();

  statistics.printTotalTraces();

  return 0;
}
