/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
//
// Copyright (c) 2006 Georgia Tech Research Corporation
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation;
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author: George F. Riley<riley@ece.gatech.edu>
//


/*
--------------------------------------------
Code edited by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
--------------------------------------------
*/


#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/node.h"
#include "ns3/addons-module.h"
#include "ns3/mobility-module.h"
#include "ns3/mobility-model.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "myonoff.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/boolean.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MyOnOff");

NS_OBJECT_ENSURE_REGISTERED (MyOnOff);

TypeId
MyOnOff::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MyOnOff")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<MyOnOff> ()
    .AddAttribute ("DataRate", "The data rate in on state.",
                   DataRateValue (DataRate ("500kb/s")),
                   MakeDataRateAccessor (&MyOnOff::m_cbrRate),
                   MakeDataRateChecker ())
    .AddAttribute ("PacketSize", "The size of packets sent in on state",
                   UintegerValue (512),
                   MakeUintegerAccessor (&MyOnOff::m_pktSize),
                   MakeUintegerChecker<uint32_t> (1))
    .AddAttribute ("Remote", "The address of the destination",
                   AddressValue (),
                   MakeAddressAccessor (&MyOnOff::m_peer),
                   MakeAddressChecker ())
    .AddAttribute ("Local",
                   "The Address on which to bind the socket. If not set, it is generated automatically.",
                   AddressValue (),
                   MakeAddressAccessor (&MyOnOff::m_local),
                   MakeAddressChecker ())
    .AddAttribute ("OnTime", "A RandomVariableStream used to pick the duration of the 'On' state.",
                   StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                   MakePointerAccessor (&MyOnOff::m_onTime),
                   MakePointerChecker <RandomVariableStream>())
    .AddAttribute ("OffTime", "A RandomVariableStream used to pick the duration of the 'Off' state.",
                   StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                   MakePointerAccessor (&MyOnOff::m_offTime),
                   MakePointerChecker <RandomVariableStream>())
    .AddAttribute ("MaxBytes", 
                   "The total number of bytes to send. Once these bytes are sent, "
                   "appNode packet is sent again, even in on state. The value zero means "
                   "that there is appNode limit.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&MyOnOff::m_maxBytes),
                   MakeUintegerChecker<uint64_t> ())
    .AddAttribute ("Protocol", "The type of protocol to use. This should be "
                   "a subclass of ns3::SocketFactory",
                   TypeIdValue (UdpSocketFactory::GetTypeId ()),
                   MakeTypeIdAccessor (&MyOnOff::m_tid),
                   // This should check for SocketFactory as a parent
                   MakeTypeIdChecker ())
    .AddAttribute ("EnableSeqTsSizeHeader",
                   "Enable use of SeqTsSizeHeader for sequence number and timestamp",
                   BooleanValue (false),
                   MakeBooleanAccessor (&MyOnOff::m_enableSeqTsSizeHeader),
                   MakeBooleanChecker ())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&MyOnOff::m_txTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("TxTraces", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&MyOnOff::m_myTxTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("MaliciousTxTraces", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&MyOnOff::m_myMaliciousTxTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("TxWithAddresses", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&MyOnOff::m_txTraceWithAddresses),
                     "ns3::Packet::TwoAddressTracedCallback")
    .AddTraceSource ("TxWithSeqTsSize", "A new packet is created with SeqTsSizeHeader",
                     MakeTraceSourceAccessor (&MyOnOff::m_txTraceWithSeqTsSize),
                     "ns3::PacketSink::SeqTsSizeCallback")
  ;
  return tid;
}


MyOnOff::MyOnOff ()
  : m_socket (0),
    m_connected (false),
    m_residualBits (0),
    m_lastStartTime (Seconds (0)),
    m_totBytes (0),
    m_unsentPacket (0)
{
  NS_LOG_FUNCTION (this);
}

MyOnOff::~MyOnOff()
{
  NS_LOG_FUNCTION (this);
}

void 
MyOnOff::SetMaxBytes (uint64_t maxBytes)
{
  NS_LOG_FUNCTION (this << maxBytes);
  m_maxBytes = maxBytes;
}

Ptr<Socket>
MyOnOff::GetSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socket;
}

int64_t 
MyOnOff::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_onTime->SetStream (stream);
  m_offTime->SetStream (stream + 1);
  return 2;
}

void
MyOnOff::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  CancelEvents ();
  m_socket = 0;
  m_unsentPacket = 0;
  // chain up
  Application::DoDispose ();
}

// Application Methods
void MyOnOff::StartApplication () // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);

  // Create the socket if not already
  if (!m_socket)
    {
      m_socket = Socket::CreateSocket (GetNode (), m_tid);
      int ret = -1;

      if (! m_local.IsInvalid())
        {
          NS_ABORT_MSG_IF ((Inet6SocketAddress::IsMatchingType (m_peer) && InetSocketAddress::IsMatchingType (m_local)) ||
                           (InetSocketAddress::IsMatchingType (m_peer) && Inet6SocketAddress::IsMatchingType (m_local)),
                           "Incompatible peer and local address IP version");
          ret = m_socket->Bind (m_local);
        }
      else
        {
          if (Inet6SocketAddress::IsMatchingType (m_peer))
            {
              ret = m_socket->Bind6 ();
            }
          else if (InetSocketAddress::IsMatchingType (m_peer) ||
                   PacketSocketAddress::IsMatchingType (m_peer))
            {
              ret = m_socket->Bind ();
            }
        }

      if (ret == -1)
        {
          NS_FATAL_ERROR ("Failed to bind socket");
        }

      m_socket->Connect (m_peer);
      m_socket->SetAllowBroadcast (true);
      m_socket->ShutdownRecv ();

      m_socket->SetConnectCallback (
        MakeCallback (&MyOnOff::ConnectionSucceeded, this),
        MakeCallback (&MyOnOff::ConnectionFailed, this));
    }
  m_cbrRateFailSafe = m_cbrRate;

  // Insure appNode pending event
  CancelEvents ();
  // If we are not yet connected, there is nothing to do here
  // The ConnectionComplete upcall will start timers at that time
  //if (!m_connected) return;
  ScheduleStartEvent ();

  GetNode()->SetState(0);
}

void MyOnOff::StopApplication () // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);

  CancelEvents ();
  if(m_socket != 0)
    {
      m_socket->Close ();
    }
  else
    {
      NS_LOG_WARN ("MyOnOff found null socket to close in StopApplication");
    }
}

void MyOnOff::CancelEvents ()
{
  NS_LOG_FUNCTION (this);

  if (m_sendEvent.IsRunning () && m_cbrRateFailSafe == m_cbrRate )
    { // Cancel the pending send packet event
      // Calculate residual bits since last packet sent
      Time delta (Simulator::Now () - m_lastStartTime);
      int64x64_t bits = delta.To (Time::S) * m_cbrRate.GetBitRate ();
      m_residualBits += bits.GetHigh ();
    }
  m_cbrRateFailSafe = m_cbrRate;
  Simulator::Cancel (m_sendEvent);
  Simulator::Cancel (m_startStopEvent);
  // Canceling events may cause discontinuity in sequence number if the
  // SeqTsSizeHeader is header, and m_unsentPacket is true
  if (m_unsentPacket)
    {
      NS_LOG_DEBUG ("Discarding cached packet upon CancelEvents ()");
    }
  m_unsentPacket = 0;
}

// Event handlers
void MyOnOff::StartSending ()
{
  NS_LOG_FUNCTION (this);
  m_lastStartTime = Simulator::Now ();
  ScheduleNextTx ();  // Schedule the send packet event
  ScheduleStopEvent ();
}

void MyOnOff::StopSending ()
{
  NS_LOG_FUNCTION (this);
  CancelEvents ();

  ScheduleStartEvent ();
}

// Private helpers
void MyOnOff::ScheduleNextTx ()
{
  NS_LOG_FUNCTION (this);

  if (m_maxBytes == 0 || m_totBytes < m_maxBytes)
    {
     // NS_ABORT_MSG_IF (m_residualBits > m_pktSize * 8, "Calculation to compute next send time will overflow");
      uint32_t bits = m_pktSize * 8 - m_residualBits;
      NS_LOG_LOGIC ("bits = " << bits);
     // Time nextTime (Seconds (bits /
     //                         static_cast<double>(m_cbrRate.GetBitRate ()))); // Time till next packet
      Time nextTime (Seconds (0.1));
      NS_LOG_LOGIC ("nextTime = " << nextTime.As (Time::S));
      m_sendEvent = Simulator::Schedule (nextTime,
                                         &MyOnOff::SendPacket, this);
    }
  else
    { // All done, cancel any pending events
      StopApplication ();
    }
}

void MyOnOff::ScheduleStartEvent ()
{  // Schedules the event to start sending data (switch to the "On" state)
  NS_LOG_FUNCTION (this);

  Time offInterval = Seconds (m_offTime->GetValue ());
  NS_LOG_LOGIC ("start at " << offInterval.As (Time::S));
  m_startStopEvent = Simulator::Schedule (offInterval, &MyOnOff::StartSending, this);
}

void MyOnOff::ScheduleStopEvent ()
{  // Schedules the event to stop sending data (switch to "Off" state)
  NS_LOG_FUNCTION (this);

  Time onInterval = Seconds (m_onTime->GetValue ());
  NS_LOG_LOGIC ("stop at " << onInterval.As (Time::S));
  m_startStopEvent = Simulator::Schedule (onInterval, &MyOnOff::StopSending, this);
}

void MyOnOff::ConnectionSucceeded (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  m_connected = true;
}

void MyOnOff::ConnectionFailed (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  NS_FATAL_ERROR ("Can't connect");
}

void MyOnOff::SendPacket ()
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT (m_sendEvent.IsExpired ());

  Ptr<Packet> packet;
  if (m_unsentPacket)
    { packet = m_unsentPacket; }
  else if (m_enableSeqTsSizeHeader) {

      Address from, to;
      m_socket->GetSockName (from);
      m_socket->GetPeerName (to);

      SeqTsSizeHeader header;
      header.SetSeq (m_seq++);
      header.SetSize (m_pktSize);
      NS_ABORT_IF (m_pktSize < header.GetSerializedSize ());
      packet = Create<Packet> (m_pktSize - header.GetSerializedSize ());
      // Trace before adding header, for consistency with PacketSink
      m_txTraceWithSeqTsSize (packet, from, to, header);
      packet->AddHeader (header);
    }
  else
    { packet = Create<Packet> (m_pktSize); }


/*
--------------------------------
    Beginning of edited code
--------------------------------
*/

  int actual;
  double timeNow =  Simulator::Now().ToDouble(Time::S);
  Ptr<Node> appNode = GetNode();
  Vector presentPosition = GetNodePosition();
  vector<Ipv4Address> neighborhoodIPList; 
  vector<Ipv4Address>::iterator it;

  // Check if node is malicious

  if(appNode->GetState() != 0)
    { m_malicious = true; }


  // Decrease the quality of every neighbor before sending any packet

  neighborhoodIPList = appNode->GetNeighborIpList();
  it = neighborhoodIPList.begin();
    
  while(it  != neighborhoodIPList.end()) {

    appNode->SetNeighborQuality(*it,appNode->GetNeighborQuality(*it)-1);

    if(appNode->GetNeighborQuality(*it) == 0 ) {
    

      // If node is on the malicious list, do not unregister it (removed)
                  // if(!appNode->IsAlreadyMaliciousNeighbor(*it))
                  //   {appNode->UnregisterNeighbor(*it);}
                  // else 
                  //   {appNode->SetNeighborQuality(*it,3);}
      
      appNode->UnregisterNeighbor(*it);



      if(it != neighborhoodIPList.end())
        {it++;}
        
      // Check if there are neighbors incorrectly identified as hop 2 or greater by checking all neighbors' distances
    } else if((appNode->GetNeighborHop(*it) > 1 ) && ( appNode->GetNeighborDistance(*it) < 115)) {

      appNode->SetNeighborHop(*it,1);   
      if(it != neighborhoodIPList.end())
        {it++;}   
      } else 
        {it++;}
  }

  // Check if there is no neighbors with hop 1

  bool noCloseNeighbors = true;
  neighborhoodIPList = appNode->GetNeighborIpList();

  for(unsigned int i = 0; i < neighborhoodIPList.size(); i++) {
  
  if( appNode->GetNeighborHop(neighborhoodIPList.at(i)) == 1)
    { noCloseNeighbors = false; }
  }


  // Send packets based on the size of the neighborhood and node mobility


  // Send a broadcast if: there is no neighbors, or its the first time sending packets, 
  //  or there is appNode neighbor near the node
  if( !appNode->IsThereAnyNeighbor() || !m_oprStarted || noCloseNeighbors ) { 

    m_oprStarted = true;
    appNode->SetPosition(presentPosition);

    // if(!appNode->IsThereAnyNeighbor())
    //   {std::cout << timeNow << '\t' << m_nodeIP << " has no neighbors" << std::endl << std::endl;}
    //std::cout << timeNow << '\t' << m_nodeIP << " sent broadcast " << std::endl << std::endl;

    
    vector<Ipv4Address> neighborhoodIPList = appNode->GetNeighborIpList();
    vector<MyTag::NeighInfos>neighborhood;
    MyTag::NeighInfos neighbor;
    Vector position;
  
    if(!m_malicious) {
      for(unsigned int i = 0; i < neighborhoodIPList.size(); i++) {

        neighbor.ip = neighborhoodIPList.at(i);
        position = appNode->GetNeighborPosition(neighbor.ip);
        neighbor.x = position.x;
        neighbor.y = position.y;
        neighbor.z = position.z;
        neighbor.infoTime = timeNow;
        neighbor.hop = appNode->GetNeighborHop(neighbor.ip);
        neighborhood.push_back(neighbor);
      }
    }

    MyTag tag;
    tag.SetPosition(GetNodePosition());
    tag.SetSimpleValue(0);
    tag.SetMessageTime(Simulator::Now().ToDouble(Time::S));
    tag.SetNeighInfosVector(neighborhood);
    tag.SetNNeighbors(neighborhood.size());

    // Send and trace packet

    packet->AddPacketTag(tag);
    actual = m_socket->Send (packet);
    if(!m_malicious)
        {m_myTxTrace(m_nodeIP,InetSocketAddress::ConvertFrom(m_peer).GetIpv4 (),tag, tag.GetMessageTime(), GetNode());}
      else  
        {m_myMaliciousTxTrace(m_nodeIP,InetSocketAddress::ConvertFrom(m_peer).GetIpv4 (),tag, tag.GetMessageTime(), GetNode());}
  }
  else if (presentPosition == appNode->GetPosition()) // Dont send packets if node is standing still
    {;}
  else {

    // If the node is moving, update the neighbours with hop 1 about the node's position and its neighborhood

    appNode->SetPosition(presentPosition);
    // std::cout << timeNow << '\t' << m_nodeIP << " sent update " << std::endl << std::endl;


    vector<Ipv4Address> neighborhoodIPList = appNode->GetNeighborIpList();
    vector<MyTag::NeighInfos>neighborhood;
    MyTag::NeighInfos neighbor;
    Vector position;
  
    if(!m_malicious) {
      for(unsigned int i = 0; i < neighborhoodIPList.size(); i++) {

        neighbor.ip = neighborhoodIPList.at(i);
        position = appNode->GetNeighborPosition(neighbor.ip);
        neighbor.x = position.x;
        neighbor.y = position.y;
        neighbor.z = position.z;
        neighbor.infoTime = timeNow;
        neighbor.hop = appNode->GetNeighborHop(neighbor.ip);
        neighborhood.push_back(neighbor);
      }
    }

    MyTag tag;
    tag.SetSimpleValue(2);
    tag.SetNeighInfosVector(neighborhood);
    tag.SetNNeighbors(neighborhood.size());

    // Send Unicasts and trace packet

    Ptr<Socket> my_socket = Socket::CreateSocket (appNode, UdpSocketFactory::GetTypeId ());
    my_socket->Bind();


    for(unsigned int i = 0; i < neighborhoodIPList.size(); i++) {

  
      if(appNode->GetNeighborHop(neighborhoodIPList.at(i)) == 1 ) {            
        
        // Send message only to close, trustworthy neighbors

        tag.SetPosition(GetNodePosition());
        tag.SetMessageTime(Simulator::Now().ToDouble(Time::S));
        packet->AddPacketTag(tag);
        actual = my_socket->SendTo(packet,0,InetSocketAddress(neighborhoodIPList.at(i), 9));

        if(actual < 0)
          {std::cout << "Error sending packet in MyOnOff Update";}
          
        if(!m_malicious)
          {m_myTxTrace(m_nodeIP, neighborhoodIPList.at(i), tag, tag.GetMessageTime(), GetNode());}
        else
          {m_myMaliciousTxTrace(m_nodeIP, neighborhoodIPList.at(i), tag, tag.GetMessageTime(), GetNode());}
        packet->RemovePacketTag(tag);
      }
    }
    my_socket->Close();
  }

  if ((unsigned) actual == m_pktSize)
    {
      m_txTrace (packet);
      m_totBytes += m_pktSize;
      m_unsentPacket = 0;
      Address localAddress;
      m_socket->GetSockName (localAddress);
      if (InetSocketAddress::IsMatchingType (m_peer))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().As (Time::S)
                       << " on-off application sent "
                       <<  packet->GetSize () << " bytes to "
                       << InetSocketAddress::ConvertFrom(m_peer).GetIpv4 ()
                       << " port " << InetSocketAddress::ConvertFrom (m_peer).GetPort ()
                       << " total Tx " << m_totBytes << " bytes");
          m_txTraceWithAddresses (packet, localAddress, InetSocketAddress::ConvertFrom (m_peer));
        }
      else if (Inet6SocketAddress::IsMatchingType (m_peer))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().As (Time::S)
                       << " on-off application sent "
                       <<  packet->GetSize () << " bytes to "
                       << Inet6SocketAddress::ConvertFrom(m_peer).GetIpv6 ()
                       << " port " << Inet6SocketAddress::ConvertFrom (m_peer).GetPort ()
                       << " total Tx " << m_totBytes << " bytes");
          m_txTraceWithAddresses (packet, localAddress, Inet6SocketAddress::ConvertFrom(m_peer));
        }
    }
  else
    {
      NS_LOG_DEBUG ("Unable to send packet; actual " << actual << " size " << m_pktSize << "; caching for later attempt");
      m_unsentPacket = packet;
    }

  m_residualBits = 0;
  m_lastStartTime = Simulator::Now ();
  ScheduleNextTx ();
}

/*
-------------------------------------
      Added Functions
-------------------------------------
*/


Address MyOnOff::GetNodeIpAddress() {
  Ptr<Node> PtrNode = this->GetNode();
  Ptr<Ipv4> ipv4 = PtrNode->GetObject<Ipv4>();
  Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1, 0);
  Ipv4Address ipAddr = iaddr.GetLocal();
  return (InetSocketAddress(ipAddr, 9));
}

void MyOnOff::Setup(Address address, uint32_t protocolId, unsigned seed) {
  NS_LOG_FUNCTION(this);
  m_peer = address;
  m_node = GetNodeIpAddress();
  m_nodeIP = InetSocketAddress::ConvertFrom(m_node).GetIpv4();
  m_oprStarted = false;
  m_malicious = false;
  m_seed = seed;

  // Choose protocol
  if (protocolId == 1) // 1 Udp
    m_tid = ns3::UdpSocketFactory::GetTypeId();
  else // 2 tcp
    m_tid = ns3::TcpSocketFactory::GetTypeId();
}

Vector MyOnOff::GetNodePosition() {

  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_sendEvent.IsExpired ());

  Ptr<Node> ThisNode = this->GetNode();
  Ptr<MobilityModel> position = ThisNode->GetObject<MobilityModel> (); // Check node current position - 26Sep2022
  NS_ASSERT (position != 0);

  return(position->GetPosition());
}

void MyOnOff::printNeighbor(Ipv4Address ip) {

    Ptr<Node> n = GetNode();
    
    int att = 0;
    int tagAtt = n->GetNeighborAttitude(ip);

    if( (int) tagAtt >= 128) 
      att = tagAtt - 256;
    else if( (int) tagAtt == 0 )
      att = 0;
    else
      {att = tagAtt;}

      att++;
      std::cout << "Position: " << n->GetNeighborPosition(ip) << "\tQuality: " << (int) n->GetNeighborQuality(ip)
      << "\tTimestamp: " << n->GetNeighborInfoTime(ip) << "\tAttitude: " << att
      << "\tHop: " << (int) n->GetNeighborHop(ip) << std::endl << std::endl;
}

int MyOnOff::generateRandomNumber(int max_value) {
  
  double number = rand() % max_value + 1;
  number += (rand() % 100) / 100; 

  return number;
}

} // Namespace ns3
