/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2007 University of Washington
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author:  Tom Henderson (tomhend@u.washington.edu)
 */

/*
--------------------------------------------
Code edited by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
--------------------------------------------
*/


#include "ns3/address.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/node.h"
#include "ns3/statistics.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/tcp-socket-factory.h"
#include "mypktsink.h"
#include "ns3/boolean.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MyPktSink");

NS_OBJECT_ENSURE_REGISTERED (MyPktSink);

TypeId 
MyPktSink::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MyPktSink")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<MyPktSink> ()
    .AddAttribute ("Local",
                   "The Address on which to Bind the rx socket.",
                   AddressValue (),
                   MakeAddressAccessor (&MyPktSink::m_local),
                   MakeAddressChecker ())
    .AddAttribute ("Protocol",
                   "The type id of the protocol to use for the rx socket.",
                   TypeIdValue (UdpSocketFactory::GetTypeId ()),
                   MakeTypeIdAccessor (&MyPktSink::m_tid),
                   MakeTypeIdChecker ())
    .AddAttribute ("EnableSeqTsSizeHeader",
                   "Enable optional header tracing of SeqTsSizeHeader",
                   BooleanValue (false),
                   MakeBooleanAccessor (&MyPktSink::m_enableSeqTsSizeHeader),
                   MakeBooleanChecker ())
    .AddTraceSource ("Rx",
                     "A packet has been received",
                     MakeTraceSourceAccessor (&MyPktSink::m_rxTrace),
                     "ns3::Packet::AddressTracedCallback")
    .AddTraceSource ("RxTraces",
                     "A packet has been received",
                     MakeTraceSourceAccessor (&MyPktSink::m_myRxTrace),
                     "ns3::Packet::AddressTracedCallback")
    .AddTraceSource ("TxTraces",
                     "A packet has been received and and answer sent",
                     MakeTraceSourceAccessor (&MyPktSink::m_myTxTrace),
                     "ns3::Packet::AddressTracedCallback")
    .AddTraceSource ("MaliciousTxTraces",
                     "A packet has been received and and malicious answer sent",
                     MakeTraceSourceAccessor (&MyPktSink::m_myMaliciousTxTrace),
                     "ns3::Packet::AddressTracedCallback")
    .AddTraceSource ("MaliciousRxTraces",
                     "A packet has been received and and malicious answer sent",
                     MakeTraceSourceAccessor (&MyPktSink::m_myMaliciousRxTrace),
                     "ns3::Packet::AddressTracedCallback")
    .AddTraceSource ("SuspiciousTraces",
                     "A packet has been received and and malicious answer sent",
                     MakeTraceSourceAccessor (&MyPktSink::m_mySuspiciousTrace),
                     "ns3::Packet::AddressTracedCallback")
    .AddTraceSource ("RxWithAddresses", "A packet has been received",
                     MakeTraceSourceAccessor (&MyPktSink::m_rxTraceWithAddresses),
                     "ns3::Packet::TwoAddressTracedCallback")
    .AddTraceSource ("RxWithSeqTsSize",
                     "A packet with SeqTsSize header has been received",
                     MakeTraceSourceAccessor (&MyPktSink::m_rxTraceWithSeqTsSize),
                     "ns3::MyPktSink::SeqTsSizeCallback")
  ;
  return tid;
}

MyPktSink::MyPktSink ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_totalRx = 0;
}

MyPktSink::~MyPktSink()
{
  NS_LOG_FUNCTION (this);
}

uint64_t MyPktSink::GetTotalRx () const
{
  NS_LOG_FUNCTION (this);
  return m_totalRx;
}

Ptr<Socket>
MyPktSink::GetListeningSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socket;
}

std::list<Ptr<Socket> >
MyPktSink::GetAcceptedSockets (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socketList;
}

void MyPktSink::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_socketList.clear ();

  // chain up
  Application::DoDispose ();
}

// Application Methods
void MyPktSink::StartApplication ()    // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);
  // Create the socket if not already
  if (!m_socket)
    {
      m_socket = Socket::CreateSocket (GetNode (), m_tid);
      if (m_socket->Bind (m_local) == -1)
        {
          NS_FATAL_ERROR ("Failed to bind socket");
        }
      m_socket->Listen ();
      m_socket->ShutdownSend ();
      if (addressUtils::IsMulticast (m_local))
        {
          Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket);
          if (udpSocket)
            {
              // equivalent to setsockopt (MCAST_JOIN_GROUP)
              udpSocket->MulticastJoinGroup (0, m_local);
            }
          else
            {
              NS_FATAL_ERROR ("Error: joining multicast on a non-UDP socket");
            }
        }
    }

  m_socket->SetRecvCallback (MakeCallback (&MyPktSink::HandleRead, this));
  m_socket->SetAcceptCallback (
    MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
    MakeCallback (&MyPktSink::HandleAccept, this));
  m_socket->SetCloseCallbacks (
    MakeCallback (&MyPktSink::HandlePeerClose, this),
    MakeCallback (&MyPktSink::HandlePeerError, this));
}

void MyPktSink::StopApplication ()     // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);
  while(!m_socketList.empty ()) //these are accepted sockets, close them
    {
      Ptr<Socket> acceptedSocket = m_socketList.front ();
      m_socketList.pop_front ();
      acceptedSocket->Close ();
    }
  if (m_socket) 
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
}

void MyPktSink::PacketReceived (const Ptr<Packet> &p, const Address &from, const Address &localAddress) {
  
  SeqTsSizeHeader header;
  Ptr<Packet> buffer;

  auto itBuffer = m_buffer.find (from);
  if (itBuffer == m_buffer.end ())
    {
      itBuffer = m_buffer.insert (std::make_pair (from, Create<Packet> (0))).first;
    }

  buffer = itBuffer->second;
  buffer->AddAtEnd (p);
  buffer->PeekHeader (header);

  NS_ABORT_IF (header.GetSize () == 0);

  while (buffer->GetSize () >= header.GetSize ())
    {
      NS_LOG_DEBUG ("Removing packet of size " << header.GetSize () << " from buffer of size " << buffer->GetSize ());
      Ptr<Packet> complete = buffer->CreateFragment (0, static_cast<uint32_t> (header.GetSize ()));
      buffer->RemoveAtStart (static_cast<uint32_t> (header.GetSize ()));

      complete->RemoveHeader (header);

      m_rxTraceWithSeqTsSize (complete, from, localAddress, header);

      if (buffer->GetSize () > header.GetSerializedSize ())
        {
          buffer->PeekHeader (header);
        }
      else
        {
          break;
        }
    }
}

void MyPktSink::HandlePeerClose (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}
 
void MyPktSink::HandlePeerError (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}

void MyPktSink::HandleAccept (Ptr<Socket> s, const Address& from)
{
  NS_LOG_FUNCTION (this << s << from);
  s->SetRecvCallback (MakeCallback (&MyPktSink::HandleRead, this));
  m_socketList.push_back (s);
}

void MyPktSink::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  while ((packet = socket->RecvFrom (from))) {
    if (packet->GetSize () == 0)
      { break; }   //EOF

    m_totalRx += packet->GetSize ();

    if (InetSocketAddress::IsMatchingType (from)) {

        NS_LOG_INFO ("At time " << Simulator::Now ().As (Time::S) << " packet sink received "
                      <<  packet->GetSize () << " bytes from " << InetSocketAddress::ConvertFrom(from).GetIpv4 ()
                      << " port " << InetSocketAddress::ConvertFrom (from).GetPort ()  << " total Rx " << m_totalRx << " bytes");
      }
    else if (Inet6SocketAddress::IsMatchingType (from)) {

      NS_LOG_INFO ("At time " << Simulator::Now ().As (Time::S)
                    << " packet sink received " <<  packet->GetSize () << " bytes from "
                    << Inet6SocketAddress::ConvertFrom(from).GetIpv6 () << " port " << Inet6SocketAddress::ConvertFrom (from).GetPort ()
                    << " total Rx " << m_totalRx << " bytes");
      }
    
    socket->GetSockName (localAddress);
    m_rxTrace (packet, from);
    m_rxTraceWithAddresses (packet, from, localAddress);


/*
--------------------------------
    Beginning of edited code
--------------------------------
*/

  // Recover tag from packet and trace it

  MyTag tag;
  packet->PeekPacketTag(tag);
  Ipv4Address sender = InetSocketAddress::ConvertFrom(from).GetIpv4 ();

  // Handle Packet content

  int attitude;
  int simpleValue = tag.GetSimpleValue();
  
  double timeDif;
  double oldDistance;
  double timeNow = Simulator::Now().ToDouble(Time::S);
  double latency = timeNow - tag.GetMessageTime();
  double messageDistance = GetDistanceFrom(tag.GetPosition());

  MyTag answerTag;
  Vector position;
  Vector tagPosition = tag.GetPosition();
  double distanceFromTag = GetDistanceFrom(tag.GetPosition());
  Ptr<Socket> my_socket;
  Ptr<Node> appNode = GetNode();
  Ptr<Packet> answerPacket = Create<Packet> (100);
  MyTag::NeighInfos aNeighbor;
  std::list<Ipv4Address> sendersIpList;
  std::vector<Ipv4Address> outputList;
  Ipv4Address receivedIp;

  vector<MyTag::NeighInfos>neighborhood = tag.GetNeighInfosVector();
  vector<Ipv4Address> neighborhoodIPList = appNode->GetNeighborIpList();
  
  // Check if node must show malicious activities

  if(appNode->GetState() != 0)
    { m_malicious = true; }


  std::cout << std::endl << timeNow << '\t' << m_nodeIP << " received tag " << (int) tag.GetSimpleValue() << " from " << sender << " ( " << messageDistance << "m away )" << std::endl;

  if(messageDistance > 100)
    {std::cout << "MENSAGEM DE LONGA DISTANCIA!!!" << std::endl << std::endl << std::endl << std::endl << std::endl;}
  // printNeighborList();


  if(neighborhood.size() == 0 && simpleValue != 0) {

    std::cout << std::endl << std::endl << "Received malicious message" << std::endl << std::endl;

    if(!appNode->IsAlreadyMaliciousNeighbor(sender)) {
      
      appNode->RegisterMaliciousNeighbor(sender,0,1,timeNow);
      appNode->SetMaliciousNeighborTimestamp(sender,timeNow);
      if(!appNode->GetMaliciousNeighborState(sender))
        appNode->IncreaseMaliciousNeighborMsgReceived(sender);
      std::cout << "Registered " << sender << " as malicious\n";

      if(!appNode->IsAlreadyNeighborSender(sender,m_nodeIP))
        { appNode->addNeighborSender(sender,m_nodeIP); }

      m_mySuspiciousTrace(appNode,sender,timeNow, latency,0,false);
      notifyAboutNeighbor(sender,3);
      checkIfMaliciousNeighbor(sender);
      
    } else if(!appNode->GetMaliciousNeighborState(sender)) {
        checkIfMaliciousNeighbor(sender);
    }    
    
  } else if (appNode->IsAlreadyMaliciousNeighbor(sender) && neighborhood.size() > 0 && appNode->IsAlreadyNeighborSender(sender,m_nodeIP)) {
    appNode->UnregisterMaliciousNeighbor(sender);
    notifyAboutNeighbor(sender,4);
    std::cout << "Node " << sender << " set as unsuspicious"<< "\n";
  }
      

  switch (simpleValue) {
  case 0:         

    // The packet received is a broadcast. Update neighborhood and send an answer packet
    // with this node's position and neighborhood

    if(!appNode->IsAlreadyNeighbor(sender))
      { appNode->RegisterNeighbor(sender,tagPosition,distanceFromTag,0,2,1,timeNow); }
    else {
      oldDistance = appNode->GetNeighborDistance(sender);
      timeDif = appNode->GetNeighborInfoTime(sender) - timeNow;
      attitude = checkAttitude(oldDistance,distanceFromTag);
      appNode->UpdateNeighbor( sender,tagPosition,distanceFromTag, attitude,2,1,timeNow);
    }
 
    // Handle received neighbor list

    for(unsigned int i = 0; i < neighborhood.size(); i++) {

      aNeighbor = neighborhood.at(i);

      if(aNeighbor.ip == m_nodeIP)
        {continue;}

      if(appNode->IsAlreadyNeighbor(aNeighbor.ip) ) {

        position.x = aNeighbor.x;
        position.y = aNeighbor.y;
        position.z = aNeighbor.z;
        oldDistance = appNode->GetNeighborDistance(aNeighbor.ip);
        timeDif = appNode->GetNeighborInfoTime(aNeighbor.ip) - timeNow;
        attitude = checkAttitude(oldDistance,GetDistanceFrom(position));

        if(!appNode->IsAlreadyNeighborSender(aNeighbor.ip, sender))
          {appNode->addNeighborSender(aNeighbor.ip, sender);}

        if(GetDistanceFrom(position) - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 115  && !(appNode->GetNeighborHop(aNeighbor.ip) > 1)) {
          appNode->UpdateNeighbor(aNeighbor.ip, position, GetDistanceFrom(position), attitude,2,
                                  compareHop(appNode->GetNeighborHop(aNeighbor.ip),aNeighbor.hop+1),timeNow);
          //std::cout << timeNow << '\t' << m_nodeIP << " updated " << aNeighbor.ip << std::endl;
          // printNeighbor(aNeighbor.ip);
        } else 
          {appNode->UnregisterNeighbor(aNeighbor.ip);}
      }
      else {

        position.x = aNeighbor.x;
        position.y = aNeighbor.y;
        position.z = aNeighbor.z;

        if(aNeighbor.ip != m_nodeIP) 
          { appNode->RegisterNeighbor( aNeighbor.ip, position, GetDistanceFrom(position), 0, 2,aNeighbor.hop+1,timeNow); appNode->addNeighborSender(aNeighbor.ip, sender); }
      }
    }
    
    // Create answer packet

    neighborhood.clear();
    neighborhoodIPList = appNode->GetNeighborIpList();

    if(!m_malicious) {
        for(unsigned int i = 0; i < neighborhoodIPList.size(); i++) {

          aNeighbor.ip = neighborhoodIPList.at(i);
          position = appNode->GetNeighborPosition(aNeighbor.ip);
          aNeighbor.x = position.x;
          aNeighbor.y = position.y;
          aNeighbor.z = position.z;
          aNeighbor.infoTime = Simulator::Now().ToDouble(Time::S);
          aNeighbor.hop = appNode->GetNeighborHop(aNeighbor.ip);
          neighborhood.push_back(aNeighbor);
        }
      }

    answerTag.SetSimpleValue(1);
    answerTag.SetPosition(GetNodePosition());
    answerTag.SetNeighInfosVector(neighborhood);
    answerTag.SetNNeighbors(neighborhood.size());

    my_socket = Socket::CreateSocket (appNode, UdpSocketFactory::GetTypeId ());
    my_socket->Bind();
    answerTag.SetMessageTime(Simulator::Now().ToDouble(Time::S));
    answerPacket->AddPacketTag(answerTag);


    if(!my_socket->SendTo(answerPacket,0,InetSocketAddress(sender, 9)))
      {std::cout << "Error sending packet in MyPktSink broadcast";}
        
    if(!m_malicious)
      {m_myTxTrace(m_nodeIP,sender,answerTag, timeNow, GetNode());}
    else
      {m_myMaliciousTxTrace(m_nodeIP,sender,answerTag, timeNow, GetNode());}
      
    my_socket->Close();

    break;
  case 1:

    // The packet received is an answer to a broadcast. Update neighborhood only

    //std::cout << timeNow << '\t' << m_nodeIP << " received answer to broadcast from " << sender << std::endl << std::endl;

    if(!appNode->IsAlreadyNeighbor(sender))
      { appNode->RegisterNeighbor(sender,tagPosition,distanceFromTag,0,2,1,timeNow); }
    else {
      oldDistance = appNode->GetNeighborDistance(sender);
      timeDif = appNode->GetNeighborInfoTime(sender) - timeNow;
      attitude = checkAttitude(oldDistance,distanceFromTag);

      if(distanceFromTag - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 115)
        { appNode->UpdateNeighbor( sender,tagPosition,distanceFromTag, attitude,2,1,timeNow); }
      else 
        { appNode->UnregisterNeighbor(sender); }
    }

    // Handle received neighbor list

    for(unsigned int i = 0; i < neighborhood.size(); i++) {

      aNeighbor = neighborhood.at(i);

      if(aNeighbor.ip == m_nodeIP)
        {continue;}

      if(appNode->IsAlreadyNeighbor(aNeighbor.ip) ) {

        position.x = aNeighbor.x;
        position.y = aNeighbor.y;
        position.z = aNeighbor.z;
        oldDistance = appNode->GetNeighborDistance(aNeighbor.ip);
        timeDif = appNode->GetNeighborInfoTime(aNeighbor.ip) - timeNow;
        attitude = checkAttitude(oldDistance,GetDistanceFrom(position));

        if(!appNode->IsAlreadyNeighborSender(aNeighbor.ip, sender))
          {appNode->addNeighborSender(aNeighbor.ip, sender);}

        if(GetDistanceFrom(position) - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 115  && !(appNode->GetNeighborHop(aNeighbor.ip) > 1)) {
          appNode->UpdateNeighbor(aNeighbor.ip, position, GetDistanceFrom(position), attitude,2,
                                  compareHop(appNode->GetNeighborHop(aNeighbor.ip),aNeighbor.hop+1),timeNow);
          //std::cout << timeNow << '\t' << m_nodeIP << " updated " << aNeighbor.ip << std::endl;
          // printNeighbor(aNeighbor.ip);
        } else 
          {appNode->UnregisterNeighbor(aNeighbor.ip);}
      }
      else {

        position.x = aNeighbor.x;
        position.y = aNeighbor.y;
        position.z = aNeighbor.z;

        if(aNeighbor.ip != m_nodeIP) 
          { appNode->RegisterNeighbor( aNeighbor.ip, position, GetDistanceFrom(position), 0, 2,aNeighbor.hop+1,timeNow); appNode->addNeighborSender(aNeighbor.ip, sender); }
      }
    }
    break;
  case 2:
  
    // The packet received is an update. Update neighborhood only

    //std::cout << timeNow << '\t' << m_nodeIP << " received an update from " << sender << std::endl << std::endl;
    
    if(!appNode->IsAlreadyNeighbor(sender))
      { appNode->RegisterNeighbor(sender,tagPosition,distanceFromTag,0,2,1,timeNow); }
    else {
      oldDistance = appNode->GetNeighborDistance(sender);
      timeDif = appNode->GetNeighborInfoTime(sender) - timeNow;
      attitude = checkAttitude(oldDistance,distanceFromTag);

      if(distanceFromTag - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 115 )
        { appNode->UpdateNeighbor( sender,tagPosition,distanceFromTag, attitude,2,1,timeNow); }
      else 
        { appNode->UnregisterNeighbor(sender); }
    }

    // Handle received neighbor list

    for(unsigned int i = 0; i < neighborhood.size(); i++) {

      aNeighbor = neighborhood.at(i);

      if(aNeighbor.ip == m_nodeIP)
        {continue;}

      if(appNode->IsAlreadyNeighbor(aNeighbor.ip) ) {

        position.x = aNeighbor.x;
        position.y = aNeighbor.y;
        position.z = aNeighbor.z;
        oldDistance = appNode->GetNeighborDistance(aNeighbor.ip);
        timeDif = appNode->GetNeighborInfoTime(aNeighbor.ip) - timeNow;
        attitude = checkAttitude(oldDistance,GetDistanceFrom(position));

        if(!appNode->IsAlreadyNeighborSender(aNeighbor.ip, sender))
          {appNode->addNeighborSender(aNeighbor.ip, sender);}

        if(GetDistanceFrom(position) - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 115  && !(appNode->GetNeighborHop(aNeighbor.ip) > 1)) {
          appNode->UpdateNeighbor(aNeighbor.ip, position, GetDistanceFrom(position), attitude,2,
                                  compareHop(appNode->GetNeighborHop(aNeighbor.ip),aNeighbor.hop+1),timeNow);
          //std::cout << timeNow << '\t' << m_nodeIP << " updated " << aNeighbor.ip << std::endl;
          // printNeighbor(aNeighbor.ip);
        } else 
          {appNode->UnregisterNeighbor(aNeighbor.ip);}
      }
      else {

        position.x = aNeighbor.x;
        position.y = aNeighbor.y;
        position.z = aNeighbor.z;

        if(aNeighbor.ip != m_nodeIP) 
          { appNode->RegisterNeighbor( aNeighbor.ip, position, GetDistanceFrom(position), 0, 2,aNeighbor.hop+1,timeNow); appNode->addNeighborSender(aNeighbor.ip, sender); }
      }
    }
    break;
    case 3:

      // Received a malicious neighbor

      neighborhood = tag.GetNeighInfosVector();
      receivedIp = neighborhood.at(0).ip;

      std::cout << "Malicious neighbor is " << receivedIp << std::endl << std::endl;

      if(appNode->IsAlreadyMaliciousNeighbor(receivedIp)) {

        if(!appNode->IsAlreadyNeighborSender(receivedIp,sender))
          { appNode->addNeighborSender(receivedIp,sender); }

        m_mySuspiciousTrace(appNode,sender,timeNow, latency,3,true);
        appNode->SetMaliciousNeighborState(receivedIp,true);
        appNode->SetMaliciousNeighborTimeToBlock(receivedIp,timeNow - appNode->GetMaliciousNeighborTimestamp(receivedIp));

      } else if(appNode->IsAlreadyNeighbor(receivedIp)) {

        if(!appNode->IsAlreadyNeighborSender(receivedIp,sender))
          { appNode->addNeighborSender(receivedIp,sender); } 
      } 

    break;
    case 4:

      // Received a "cleared from suspicious activity" neighbor

      neighborhood = tag.GetNeighInfosVector();
      receivedIp = neighborhood.at(0).ip;

      std::cout << "Malicious neighbor received is " << receivedIp << std::endl << std::endl;

      if(appNode->IsAlreadyNeighbor(receivedIp) && !appNode->IsAlreadyNeighborSender(receivedIp,sender) ) 
        { appNode->addNeighborSender(receivedIp,sender); } 
       
    break;
    case 5:

      // Received a request to check a neighbor

      neighborhood = tag.GetNeighInfosVector();
      receivedIp = neighborhood.at(0).ip;

      if(appNode->IsAlreadyNeighbor(receivedIp)) {
          
        sendersIpList = appNode->getNeighborSenders(receivedIp);
        outputList.push_back(receivedIp);
        
        m_mySuspiciousTrace(appNode,sender,Simulator::Now().ToDouble(Time::S), latency,5,false);

        for(std::list<Ipv4Address>::iterator it = sendersIpList.begin(); it != sendersIpList.end() ; it++ ) {
          outputList.push_back(*it);
        }
        
        // Create answer packet

        neighborhood.clear();

        for(unsigned int i = 0; i < outputList.size(); i++) {
          aNeighbor.ip = outputList.at(i);
          if(appNode->IsAlreadyNeighbor(aNeighbor.ip)) {
            position = appNode->GetNeighborPosition(aNeighbor.ip);
            aNeighbor.x = position.x;
            aNeighbor.y = position.y;
            aNeighbor.z = position.z;
          } else {
            aNeighbor.x = 0;
            aNeighbor.y = 0;
            aNeighbor.z = 0;
          }
          neighborhood.push_back(aNeighbor);
        }

        neighborhoodIPList = appNode->GetNeighborIpList();

        answerTag.SetSimpleValue(6);
        answerTag.SetPosition(GetNodePosition());
        answerTag.SetNeighInfosVector(neighborhood);
        answerTag.SetNNeighbors(neighborhood.size());
        answerTag.SetMessageTime(tag.GetMessageTime());

        my_socket = Socket::CreateSocket(appNode, UdpSocketFactory::GetTypeId());
        my_socket->Bind();
        answerPacket->AddPacketTag(answerTag);


        if(appNode->IsAlreadyMaliciousNeighbor(receivedIp) && !appNode->GetMaliciousNeighborState(receivedIp))
          {appNode->IncreaseMaliciousNeighborMsgSent(receivedIp);}

        if(!my_socket->SendTo(answerPacket,0,InetSocketAddress(sender, 9)))
          {std::cout << "Error sending packet in MyPktSink neighbor check answer";}
            
        if(!m_malicious)
          {m_myTxTrace(m_nodeIP,sender,answerTag, timeNow, GetNode());}
        else
          {m_myMaliciousTxTrace(m_nodeIP,sender,answerTag, timeNow, GetNode());}
          
        my_socket->Close();
      }
    break;
    case 6:

      // Received a check for malicious neighbor 

      neighborhood = tag.GetNeighInfosVector();
      receivedIp = neighborhood.at(0).ip;            // This is the maliciousIp which is being checked

      m_mySuspiciousTrace(appNode,sender,timeNow, latency,6,true);
      
      if(receivedIp != m_nodeIP) {
        appNode->SetMaliciousNeighborState(receivedIp,true);
        appNode->SetMaliciousNeighborTimeToBlock(receivedIp,timeNow - appNode->GetMaliciousNeighborTimestamp(receivedIp));
      }

      for (unsigned int i = 1; i < neighborhood.size(); i++) {
        
        aNeighbor = neighborhood.at(i);

        if(aNeighbor.ip == m_nodeIP)
          {continue;}

        if(appNode->IsAlreadyNeighbor(aNeighbor.ip) && aNeighbor.x != 0) {

          position.x = aNeighbor.x;
          position.y = aNeighbor.y;
          position.z = aNeighbor.z;
          oldDistance = appNode->GetNeighborDistance(aNeighbor.ip);
          timeDif = appNode->GetNeighborInfoTime(aNeighbor.ip) - timeNow;
          attitude = checkAttitude(oldDistance,GetDistanceFrom(position));

          if(!appNode->IsAlreadyNeighborSender(aNeighbor.ip, sender))
            {appNode->addNeighborSender(aNeighbor.ip, sender);}

          if(GetDistanceFrom(position) - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 115  && !(appNode->GetNeighborHop(aNeighbor.ip) > 1)) {
            appNode->UpdateNeighbor(aNeighbor.ip, position, GetDistanceFrom(position), attitude,2,
                                    compareHop(appNode->GetNeighborHop(aNeighbor.ip),aNeighbor.hop+1),timeNow);
          } else 
            {appNode->UnregisterNeighbor(aNeighbor.ip);}
        }
        else if (aNeighbor.x != 0) {

          position.x = aNeighbor.x;
          position.y = aNeighbor.y;
          position.z = aNeighbor.z;
          
          appNode->RegisterNeighbor( aNeighbor.ip, position, GetDistanceFrom(position), 0, 2,aNeighbor.hop+1,timeNow); appNode->addNeighborSender(aNeighbor.ip, sender);
        }
      }
    break;
  default:
    std::cout << "Error in MyPktSink, SimpleValue not found." << std::endl;
    break;
}

  if(!appNode->IsAlreadyNeighborSender(sender,m_nodeIP)) {
    appNode->addNeighborSender(sender, m_nodeIP);
  } 

  appNode->printNeighborsSendersList(std::cout);

  if(!m_malicious)
    {m_myRxTrace(m_nodeIP, sender,InetSocketAddress::ConvertFrom(localAddress).GetIpv4 (),tag, timeNow, latency, GetNode());}
  else
    {m_myMaliciousRxTrace(m_nodeIP, sender,InetSocketAddress::ConvertFrom(localAddress).GetIpv4 (),tag, timeNow, GetNode());}

  //printNeighborList();
    
  if (m_enableSeqTsSizeHeader)
    { PacketReceived (packet, from, localAddress); }
  }
}


/* -----------------------
      Added Functions
   ----------------------- */


Address MyPktSink::GetNodeIpAddress() {
  Ptr<Node> PtrNode = this->GetNode();
  Ptr<Ipv4> ipv4 = PtrNode->GetObject<Ipv4>();
  Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1, 0);
  Ipv4Address ipAddr = iaddr.GetLocal();
  return (InetSocketAddress(ipAddr, 9));
}

void MyPktSink::Setup(Address toAddress, uint32_t protocolId, unsigned seed) {

  NS_LOG_FUNCTION(this);
  m_node = GetNodeIpAddress();
  m_nodeIP = InetSocketAddress::ConvertFrom(m_node).GetIpv4();
  m_local = toAddress;
  m_socket = 0;
  m_totalRx = 0;
  m_malicious = false;
  m_seed = seed;

  if (protocolId == 1) // 1 Udp
    m_tid = ns3::UdpSocketFactory::GetTypeId();
  else // 2 tcp
    m_tid = ns3::TcpSocketFactory::GetTypeId();  
}

Vector MyPktSink::GetNodePosition() {

  NS_LOG_FUNCTION (this);
  Ptr<Node> ThisNode = this->GetNode();
  Ptr<MobilityModel> position = ThisNode->GetObject<MobilityModel> (); // Check node current position - 26Sep2022
  NS_ASSERT (position != 0);

  return(position->GetPosition ());
}

double MyPktSink::GetDistanceFrom(Vector point) {

  double distance = 0, distance1,distance2,distance3;

  distance1 = pow(point.x - GetNodePosition().x,2);
  distance2 = pow(point.y - GetNodePosition().y,2);
  distance3 = pow(point.z - GetNodePosition().z,2);

  distance = sqrt(distance1+distance2+distance3);

  return distance;
}

double MyPktSink::calculateDistanceBetweenTwoPoints(Vector a,Vector b) {

  double distance, distance1, distance2, distance3;

  distance1 = pow(a.x - b.x,2);
  distance2 = pow(a.y - b.y,2);
  distance3 = pow(a.z - b.z,2);

  distance = sqrt(distance1+distance2+distance3);

  return distance;
}

int MyPktSink::checkAttitude(double before, double after) {

  // Returns 1 if the aNeighbor is getting closer, -1 if its flying away and 0 if unchanged

  double attitude = before - after;

    if(attitude > 1)
      return 1;
    else if (attitude < 1)
      return -1;
    else
      return 0;
}

int MyPktSink::checkSpeed(double before, double after, double time) {

  double attitude = before - after;

  return (int) attitude/time;
}

uint8_t MyPktSink::compareHop(uint8_t a, uint8_t b ) {

  if( a > b )
    return b;
  else
    return a;
}

void MyPktSink::printNeighborList() {

  Ptr<Node> n = GetNode();
  vector<Ipv4Address> neighborIPList = n->GetNeighborIpList();
  std::cout << m_nodeIP << " has " << neighborIPList.size() << " neighbors" << std::endl;
  for(unsigned int i = 0; i < neighborIPList.size();i++) 
    {printNeighbor(neighborIPList.at(i));}
}

void MyPktSink::printNeighbor(Ipv4Address ip) {

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

  std::cout << "\tIP: " << ip << " Position: " << n->GetNeighborPosition(ip) << " Quality: " << (int) n->GetNeighborQuality(ip)
  << " Timestamp: " << n->GetNeighborInfoTime(ip) << " Attitude: " << att << " Hop: " << (int) n->GetNeighborHop(ip) << std::endl;

}

void MyPktSink::notifyAboutNeighbor (Ipv4Address maliciousIP, uint8_t tagType) {

  MyTag answerTag;
  MyTag::NeighInfos aNeighbor;  
  Ptr<Node> appNode = GetNode();
  vector<MyTag::NeighInfos>neighborhood;
  Ptr<Packet> answerPacket = Create<Packet> (100);
  vector<Ipv4Address> neighborsIPList = appNode->GetNeighborIpList();
  Ptr<Socket> my_socket = Socket::CreateSocket (appNode, UdpSocketFactory::GetTypeId ());

  aNeighbor.ip = maliciousIP;

  if(!m_malicious) 
    {neighborhood.push_back(aNeighbor);}

  answerTag.SetNNeighbors(1);
  answerTag.SetSimpleValue(tagType);
  answerTag.SetNeighInfosVector(neighborhood);
  answerTag.SetMessageTime(Simulator::Now().ToDouble(Time::S));

  my_socket->Bind();

  if(neighborsIPList.size() > 0 ) {

    for(unsigned int i = 0; i < neighborsIPList.size(); i++) {

      if(appNode->GetNeighborHop(neighborsIPList.at(i)) == 1 && ( neighborsIPList.at(i) != maliciousIP) ) {

        // Send message only to close, trustworthy neighbors

        answerTag.SetPosition(GetNodePosition());
        answerPacket->AddPacketTag(answerTag);
        
        int actual = my_socket->SendTo(answerPacket,0,InetSocketAddress(neighborsIPList.at(i), 9));


    if(appNode->IsAlreadyMaliciousNeighbor(maliciousIP) && !appNode->GetMaliciousNeighborState(maliciousIP) )
      {appNode->IncreaseMaliciousNeighborMsgSent(maliciousIP);}
        
        if(!m_malicious)
          {m_myTxTrace(m_nodeIP, neighborsIPList.at(i), answerTag, answerTag.GetMessageTime(), appNode);}
        else
          {m_myMaliciousTxTrace(m_nodeIP, neighborsIPList.at(i), answerTag, answerTag.GetMessageTime(), appNode);}
        if(actual < 0)
          {std::cout << "Error sending packet in MyPktSink notifyAboutNeighbor";}        
          answerPacket->RemovePacketTag(answerTag);
        }
      }
    }
  my_socket->Close();
}

void MyPktSink::checkIfMaliciousNeighbor(Ipv4Address suspectIp) {

  Vector neighborPosition;
  MyTag tag;
  Ptr<Node> appNode = GetNode();
  bool checkForNeighbors = false;
  Vector suspectPosition = appNode->GetNeighborPosition(suspectIp);
  std::list<Ipv4Address> neighborhood = appNode->getNeighborSenders(suspectIp);


  for (std::list<Ipv4Address>::iterator it = neighborhood.begin(); it != neighborhood.end(); it++) {
    if(*(it) != m_nodeIP) 
      { checkForNeighbors = true; }
  }

  
  if(checkForNeighbors) {
    
    std::cout << "Entered in check for malicious neighbor\n";

    for (std::list<Ipv4Address>::iterator it = neighborhood.begin (); it != neighborhood.end(); it++) {
    
      if(*(it) != m_nodeIP && *(it) != suspectIp ) {

        neighborPosition = appNode->GetNeighborPosition(*(it));
        double distance = calculateDistanceBetweenTwoPoints(suspectPosition,neighborPosition);

        std::cout << "Found a neighbor within: " << distance << "m\n";

        if(distance < 115) {
          appNode->SetMaliciousNeighborState(suspectIp,true);
          appNode->SetMaliciousNeighborTimeToBlock(suspectIp,Simulator::Now().ToDouble(Time::S) - appNode->GetMaliciousNeighborTimestamp(suspectIp));
          m_mySuspiciousTrace(appNode,m_nodeIP,Simulator::Now().ToDouble(Time::S), -1,-1,true);
          break;
        }
      }   
    }
  } else {

    std::cout << "Entered in check for malicious neighbor with no neighbor senders\n";

    std::vector<ns3::Ipv4Address> possibleNeighbors = buildSlidingWindow(suspectIp);

    for(unsigned int i = 0; i < possibleNeighbors.size(); i++) {

      if(possibleNeighbors.at(i) != m_nodeIP && possibleNeighbors.at(i) != suspectIp ) {

        MyTag answerTag;
        MyTag::NeighInfos aNeighbor;  
        vector<MyTag::NeighInfos>neighborhood;
        Ptr<Packet> answerPacket = Create<Packet> (100);
        Ptr<Socket> my_socket = Socket::CreateSocket (appNode, UdpSocketFactory::GetTypeId ());

        aNeighbor.ip = suspectIp;
        neighborhood.push_back(aNeighbor);

        answerTag.SetNNeighbors(1);
        answerTag.SetSimpleValue(5);
        answerTag.SetNeighInfosVector(neighborhood);
        answerTag.SetMessageTime(Simulator::Now().ToDouble(Time::S));

        my_socket->Bind();

        answerTag.SetPosition(GetNodePosition());
        answerPacket->AddPacketTag(answerTag);
        
        int actual = my_socket->SendTo(answerPacket,0,InetSocketAddress(possibleNeighbors.at(i), 9));

        if(appNode->IsAlreadyMaliciousNeighbor(suspectIp) && !appNode->GetMaliciousNeighborState(suspectIp))
          {appNode->IncreaseMaliciousNeighborMsgSent(suspectIp);}
        
        
        if(!m_malicious)
          {m_myTxTrace(m_nodeIP, possibleNeighbors.at(i), answerTag, answerTag.GetMessageTime(), appNode);}
        else
          {m_myMaliciousTxTrace(m_nodeIP, possibleNeighbors.at(i), answerTag, answerTag.GetMessageTime(), appNode);}
        if(actual < 0)
          {std::cout << "Error sending packet in MyPktSink notifyAboutNeighbor";}      
  
        my_socket->Close();
      }
    }
  }
}

int MyPktSink::generateRandomNumber(int max_value) {

  double number = rand() % max_value + 1;
  number += (rand() % max_value) / 100; 

  return number;
}


std::vector<ns3::Ipv4Address> MyPktSink::buildSlidingWindow (Ipv4Address suspectIp) {

  Vector neighborPosition;
  Ptr<Node> appNode = GetNode();
  double minX, minY, maxX, maxY;
  Vector myPosition = appNode->GetPosition();
  std::vector<ns3::Ipv4Address> commonNeighbors;
  Vector maliciousPosition = GetNode()->GetNeighborPosition(suspectIp);
  std::vector<ns3::Ipv4Address> myNeighbors = appNode->GetNeighborIpList();

  if( myPosition.x < maliciousPosition.x) {
    minX = maliciousPosition.x - 115;
    maxX = myPosition.x + 115;
  } else {
    minX = myPosition.x - 115;
    maxX = maliciousPosition.x + 115;
  }

  if( myPosition.y < maliciousPosition.y) {
    minY = maliciousPosition.y - 115;
    maxY = myPosition.y + 115;
  } else {
    minY = myPosition.y - 115;
    maxY = maliciousPosition.y + 115;
  }

  for ( unsigned int i = 0; i < myNeighbors.size();i++) {
    neighborPosition = appNode->GetNeighborPosition(myNeighbors.at(i));
    if ( minX < neighborPosition.x && neighborPosition.x < maxX && appNode->GetNeighborHop(myNeighbors.at(i)) == 1 ) {
      if( minY < neighborPosition.y && neighborPosition.y < maxY ) {
        commonNeighbors.push_back(myNeighbors.at(i));
        std::cout << "FOUND COMMON NEIGHBOR: " << myNeighbors.at(i) << std::endl; 
      }
    }
  }
  
  return commonNeighbors;
}


} // Namespace ns3