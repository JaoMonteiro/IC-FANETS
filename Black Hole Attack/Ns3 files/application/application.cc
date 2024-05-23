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
//#include "ns3/netsimulyzer-module.h"


/*

    --------------------------------------------
    Code developed by Agnaldo Batista <agnaldosb@gmail.com> and João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>, April 2024
    --------------------------------------------

  To run this simulation, open a terminal in ns3's root folder (ns3-3.34/) and run a command like:

    /waf --run "simulation --numUAV=40 --maliciousDrones=1"
  
  Where "numUAV" is the desired number of UAVs in the simulation and "maliciousDrones" is the number of malicious Drones.
  If you run "" /waf --run simulation "", there will be 6 UAVs and no malicious drones.
  
  BE CAREFUL! 
	- There must exist a "Logs" folder inside ns3's root folder (ns3-3.34/), otherwise Logs will not be saved!
    - Maximum nodes is 40
    - Simulation runtime is defined at line 339
    - Depending of the number of malicious drones, they are deployed differently. Check lines 311 - 333
    - To use NetAnim, uncomment lines 255 - 265
    - To use NetSimulyzer, after installing it and its module, uncomment lines 43 - 53 and 268 - 285    

*/



using namespace ns3;

// Define callback function to track node mobility
// void CourseChanged(Ptr<netsimulyzer::XYSeries> posSeries, Ptr<netsimulyzer::LogStream> eventLog,
//                     std::string context, Ptr<const MobilityModel> model) {

//   const auto position = model->GetPosition();
//     // Write coordinates to log
//   *eventLog << Simulator::Now().GetSeconds() << " Course Change Position: [" << position.x << ", "
//             << position.y << ", " << position.z << "]\n";
//     // Add data point to XYSeries
//   posSeries->Append(position.x, position.y);
// }

void createMaliciousDrone(int node) {
  
  NodeList::GetNode(node)->SetState(1);
  std::cout << "\n\n" << Simulator::Now().ToDouble(Time::S) << " Node 1.1.1." << node + 1 << " is now malicious\n\n\n";
}

int main (int argc, char *argv[]) {
  
  //-----------------------
  // Create traces Folder
  //-----------------------

  Statistics statistics;
  time_t day = time(NULL);
  struct tm time = *localtime(&day); 
  std::stringstream location;
    // "../../../../media/joao/Simulations/" "./Logs/" 
  location << "./Logs/" << time.tm_mday << '_' << time.tm_mon+1 << '_'  << time.tm_year + 1900 << "_at_" << time.tm_hour << time.tm_min;
  mkdir(location.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  statistics.filepath = location.str();

  //-----------------------
  // Check number of nodes
  //-----------------------

   // Creating and defining seeds to be used

  unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
  //seed = 1905136375;
  srand(seed);
  RngSeedManager::SetSeed(seed);

  uint32_t numUAV = 5;
  uint32_t maliciousDrones = 0;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("numUAV", "Number of UAVs", numUAV);
  cmd.AddValue ("maliciousDrones", "Number of malicious UAVs", maliciousDrones);

  cmd.Parse (argc,argv);

  if (numUAV > 41) {
    std::cout << "Numero maximo de UAVs excedido. Tente novamente com um numero menor." << std::endl;
    return 1;
  } else if (numUAV < 2)
  {
    std::cout << "Numero minimo de UAVs nao alcancado. Tente novamente com um numero maior." << std::endl;
    return 1;
  }

  if (maliciousDrones < 0) {
    std::cout << "Numero de UAVs maliciosos alterado para 0." << std::endl;
    maliciousDrones = 0;
  } else if (maliciousDrones > numUAV)
  {
    std::cout << "Numero máximo de UAVs maliciosos alcancado. Tente novamente com um numero menor que " << numUAV << std::endl;
    return 1;
  }

std::stringstream text;
text << std::endl << "The seed is: " << seed << std::endl << std::endl;
text << "Total number of drones: " << numUAV << std::endl;
text << "Number of malicious drones: " << maliciousDrones << std::endl;
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
  double stop = 1198;
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
    start += 0.025; // Avoid to start all the OnOff together
  }

  //-----------------------
  // Traces Config
  //-----------------------

  // Callback Trace to Collect data from MyPktSink Application
  
  for (it = Nodes.Begin(); it != Nodes.End(); it++) {
    uint32_t nodeID = (*it)->GetId();
    std::ostringstream paramTest;
    std::ostringstream paramTest2;
    std::ostringstream paramTest3;
    std::ostringstream paramTest4;
    std::ostringstream paramTest5;
    paramTest << "/NodeList/" << (nodeID) << "/ApplicationList/*/$ns3::MyPktSink/RxTraces";
    paramTest2 << "/NodeList/" << (nodeID) << "/ApplicationList/*/$ns3::MyPktSink/TxTraces";
    paramTest3 << "/NodeList/" << (nodeID) << "/ApplicationList/*/$ns3::MyPktSink/MaliciousTxTraces";
    paramTest4 << "/NodeList/" << (nodeID) << "/ApplicationList/*/$ns3::MyPktSink/MaliciousRxTraces";
    paramTest5 << "/NodeList/" << (nodeID) << "/ApplicationList/*/$ns3::MyPktSink/SuspiciousTraces";
    Config::ConnectWithoutContext(paramTest.str().c_str(), MakeCallback(&Statistics::receiverCallback, &statistics));
    Config::ConnectWithoutContext(paramTest2.str().c_str(), MakeCallback(&Statistics::senderCallback, &statistics));
    Config::ConnectWithoutContext(paramTest3.str().c_str(), MakeCallback(&Statistics::maliciousSenderCallback, &statistics));
    Config::ConnectWithoutContext(paramTest4.str().c_str(), MakeCallback(&Statistics::maliciousReceiverCallback, &statistics));
    Config::ConnectWithoutContext(paramTest5.str().c_str(), MakeCallback(&Statistics::suspiciousActivityCallback, &statistics));
  }

  // Callback Trace to Collect data from MyOnOff Application
  
  for (it = Nodes.Begin(); it != Nodes.End(); it++) {
    uint32_t nodeID = (*it)->GetId();
    std::ostringstream paramTest;
    std::ostringstream paramTest2;
    paramTest << "/NodeList/" << (nodeID) << "/ApplicationList/*/$ns3::MyOnOff/TxTraces";
    paramTest2 << "/NodeList/" << (nodeID) << "/ApplicationList/*/$ns3::MyOnOff/MaliciousTxTraces";
    Config::ConnectWithoutContext(paramTest.str().c_str(), MakeCallback(&Statistics::senderCallback, &statistics));
    Config::ConnectWithoutContext(paramTest2.str().c_str(), MakeCallback(&Statistics::maliciousSenderCallback, &statistics));
  }


  // AnimationInterface anim ("animation.xml");
  // anim.AddResource("/home/joao/Downloads/uav2.png");
  // anim.AddResource("/home/joao/Downloads/uav1.png");
  // anim.SetBackgroundImage("/home/joao/Downloads/sky.png", 0, 0, 0.4, 0.4, 0.5);

  // for(unsigned int i = 0; i < NodeList::GetNNodes();i++){

  //   anim.UpdateNodeImage(i,0);
  //   anim.UpdateNodeSize(i,20,20);
  // }
  //   anim.UpdateNodeImage(1,1);

  // auto orchestrator = CreateObject<netsimulyzer::Orchestrator>("outdoor-random-walk-example.json"); // NOME DO ARQUIVO DE SAIDA
  // // Use helper to define model for visualizing nodes and aggregate to Node object
  // netsimulyzer::NodeConfigurationHelper nodeHelper{orchestrator};
  // nodeHelper.Set("Model", netsimulyzer::models::QUADCOPTER_UAV_VALUE);
  // nodeHelper.Set("Scale", DoubleValue(4));
  // nodeHelper.Install(Nodes);
  // Ptr<netsimulyzer::LogStream> eventLog = CreateObject<netsimulyzer::LogStream>(orchestrator);
  // eventLog->SetAttribute("Name", StringValue("Event Log"));
  // // Create XYSeries that will be used to display mobility (similar to a 2D plot)
  // Ptr<netsimulyzer::XYSeries> posSeries = CreateObject<netsimulyzer::XYSeries>(orchestrator);
  // posSeries->SetAttribute("Name", StringValue("Node position"));
  // posSeries->SetAttribute("LabelMode", StringValue("Hidden"));
  // posSeries->SetAttribute("Color", netsimulyzer::BLUE_VALUE);
  // posSeries->GetXAxis()->SetAttribute("Name", StringValue("X position (m)"));
  // posSeries->GetYAxis()->SetAttribute("Name", StringValue("Y position (m)"));

  // Tie together the callback function, LogStream, and XYSeries
  //Config::Connect("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeBoundCallback(&CourseChanged, posSeries, eventLog));

  //-----------------------
  // Create Threats
  //-----------------------

  // Guarantee that threats are different nodes
  vector<int> maliciousNodes;
  for( unsigned int i = 0; i < maliciousDrones;i++) {
    maliciousNodes.push_back(rand() % numUAV);
  }

  if(maliciousDrones != 1) {
    for(unsigned int i = 0; i < maliciousNodes.size();i++) {

      int num;
      do {num = rand() % numUAV; } 
        while (find(begin(maliciousNodes), end(maliciousNodes), num) != end(maliciousNodes));

      maliciousNodes[i] = num;
    }
  } else {
    maliciousNodes[0] = rand() % numUAV;
  }

  text.str("");
  text << "Malicious Activity:" << std::endl;


  // Effectively create the threats
  if(maliciousDrones == 3) {
    Simulator::Schedule(Seconds(300),&createMaliciousDrone,maliciousNodes[0]);
    statistics.addMaliciousDrone(maliciousNodes[0],300);
      text << "Node 1.1.1." << maliciousNodes[0] + 1 << " will turn malicious in " << "300s" << std::endl;
    Simulator::Schedule(Seconds(600),&createMaliciousDrone,maliciousNodes[1]);
    statistics.addMaliciousDrone(maliciousNodes[1],600);
      text << "Node 1.1.1." << maliciousNodes[1] + 1 << " will turn malicious in " << "600s" << std::endl;
    Simulator::Schedule(Seconds(900),&createMaliciousDrone,maliciousNodes[2]);
    statistics.addMaliciousDrone(maliciousNodes[2],900); 
      text << "Node 1.1.1." << maliciousNodes[2] + 1<< " will turn malicious in " << "900s" << std::endl;
  } else if (maliciousDrones == 1) {
    Simulator::Schedule(Seconds(2),&createMaliciousDrone,maliciousNodes[0]);
    statistics.addMaliciousDrone(maliciousNodes[0],2);
      text << "Node 1.1.1." << maliciousNodes[0] + 1 << " will turn malicious in " << "2s" << std::endl;
  } else {
    for( unsigned int i = 0; i < maliciousDrones;i++) {
      Time t = Seconds(rand() % 1000);
      Simulator::Schedule(t,&createMaliciousDrone,maliciousNodes[i]);
      statistics.addMaliciousDrone(maliciousNodes[i],t.ToDouble(Time::S));
      text << "Node 1.1.1." << maliciousNodes[i] + 1 << " will turn malicious in " << t.ToDouble(Time::S) << "s"<< std::endl;
    }
  } 
  
  statistics.addSimulationInfo(text.str());


  Simulator::Stop(Seconds (1200));
  statistics.addSimulationInfo("\nSimulation runtime is 1200s");
  Simulator::Run(); 
  statistics.genMaliciousTraces();
  Simulator::Destroy();

  statistics.printTotalTraces();
  return 0;
}
