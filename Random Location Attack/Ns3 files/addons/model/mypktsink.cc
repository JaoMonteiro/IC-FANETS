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

void MyPktSink::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  while ((packet = socket->RecvFrom (from))) {
    if (packet->GetSize () == 0)
      { break;}   //EOF

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
  double messageDistance = GetDistanceFrom(tag.GetPosition());

  MyTag answerTag;
  Vector position;
  Vector tagPosition = tag.GetPosition();
  Ptr<Socket> my_socket;
  Ptr<Node> appNode = GetNode();
  Ptr<Packet> answerPacket = Create<Packet> (100);
  MyTag::NeighInfos aNeighbor;

  vector<MyTag::NeighInfos>neighborhood;
  vector<Ipv4Address> neighborhoodIPList = appNode->GetNeighborIpList();

  if(appNode->GetState() != 0)
    {m_malicious = true;}

  std::cout << std::endl << timeNow << '\t' << m_nodeIP << " received tag " << (int) tag.GetSimpleValue() << " from " << sender << " ( " << messageDistance << "m away )" << std::endl;

      printNeighborList();
      printMaliciousNeighborList();
  
  if(!appNode->GetMaliciousNeighborState(sender)) {        // If sender is already blocked, ignore message
     
    if(appNode->IsAlreadyMaliciousNeighbor(sender) && messageDistance > 87) {          
      // If sender is malicious and far away increase recurrence                                                                                   
      appNode->IncreaseMaliciousNeighborRecurrence(sender);
      appNode->IncreaseMaliciousNeighborMsgReceived(sender);
      std::cout << m_nodeIP << " increased recurrence of " << sender << " to " << appNode->GetMaliciousNeighborRecurrence(sender) << std::endl;
      appNode->SetNeighborState(sender,true);

      if(appNode->GetMaliciousNeighborRecurrence(sender) == 3) {
          // Block neighbor
        appNode->SetMaliciousNeighborState(sender,true);
        std::cout <<  m_nodeIP << " Blocked " << sender << std::endl << std::endl;
        double timeToBlock =  timeNow - appNode->GetMaliciousNeighborTimestamp(sender);
        appNode->SetMaliciousNeighborTimeToBlock(sender,timeToBlock);
        appNode->UnregisterNeighbor(sender);
        notifyAboutMaliciousNeighbor(sender,4);
      } else 
        { notifyAboutMaliciousNeighbor(sender,3); }                  
    }                                                                                  
    else if (appNode->IsAlreadyMaliciousNeighbor(sender) && messageDistance <= 87) {         
      // If a valid information is received from a malicious node
      appNode->DecreaseMaliciousNeighborRecurrence(sender);
      std::cout <<  m_nodeIP << " Decreased suspicious about " << sender << std::endl << std::endl;
      notifyAboutMaliciousNeighbor(sender,6);
      if(appNode->GetMaliciousNeighborRecurrence(sender) < 1) {
        appNode->UnregisterMaliciousNeighbor(sender);
        appNode->SetNeighborState(sender,false);
        std::cout << m_nodeIP << " removed suspicious of " << sender << std::endl << std::endl;
        notifyAboutMaliciousNeighbor(sender,5);
      }
    }
    else if(messageDistance > 87) {                                                   
      // If the sender is not malicious but far away add to suspect list and notify neighbors
      std::cout << m_nodeIP << " added suspicious neighbor " << sender << std::endl << std::endl;
      if(!appNode->IsAlreadyNeighbor(sender)) 
        {appNode->RegisterNeighbor(sender,tag.GetPosition(),messageDistance,0,3,1,timeNow);}
      appNode->RegisterMaliciousNeighbor(sender,false,1,timeNow);
      appNode->IncreaseMaliciousNeighborMsgReceived(sender);
      appNode->SetNeighborState(sender,true);
      notifyAboutMaliciousNeighbor (sender,3);

    }  




    if(!appNode->GetMaliciousNeighborState(sender))             // In case sender was just blocked, ignore message.
      switch (simpleValue) {
        case 0:         

        // The packet received is a broadcast. Update neighborhood and send an answer packet
        // with this node's position and neighborhood

        //std::cout << timeNow << '\t' << m_nodeIP << " received broadcast from " << sender << std::endl << std::endl;
      
        if(!appNode->IsAlreadyNeighbor(sender)) {

          appNode->RegisterNeighbor(sender,tagPosition,GetDistanceFrom(tagPosition),0,2,1,tag.GetMessageTime());

          //std::cout << timeNow << '\t' << m_nodeIP << " added " << sender << std::endl;
          // printNeighbor(sender);


          // Handle received neighbor list

          neighborhood = tag.GetNeighInfosVector();

          for(unsigned int i = 0; i < neighborhood.size(); i++) {

            aNeighbor = neighborhood.at(i);
            if(appNode->IsAlreadyNeighbor(aNeighbor.ip)) {

              position.x = aNeighbor.x;
              position.y = aNeighbor.y;
              position.z = aNeighbor.z;
              oldDistance = appNode->GetNeighborDistance(aNeighbor.ip);
              timeDif = appNode->GetNeighborInfoTime(aNeighbor.ip) - timeNow;
              attitude = checkAttitude(oldDistance,GetDistanceFrom(position));

              if(GetDistanceFrom(position) - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 87) {
                appNode->UpdateNeighbor( aNeighbor.ip, position, GetDistanceFrom(position), attitude,2,
                                            compareHop(appNode->GetNeighborHop(aNeighbor.ip),aNeighbor.hop+1),aNeighbor.infoTime);
                //std::cout << timeNow << '\t' << m_nodeIP << " updated " << aNeighbor.ip << std::endl;
                // printNeighbor(aNeighbor.ip);
              } else 
                {appNode->UnregisterNeighbor(aNeighbor.ip);}
            }
            else {

              position.x = aNeighbor.x;
              position.y = aNeighbor.y;
              position.z = aNeighbor.z;

              if(aNeighbor.ip != m_nodeIP) {
                
                appNode->RegisterNeighbor( aNeighbor.ip, position, GetDistanceFrom(position), 0, 2,aNeighbor.hop+1,aNeighbor.infoTime);
                //std::cout << timeNow << '\t' << m_nodeIP << " added " << aNeighbor.ip << std::endl;
                // printNeighbor(aNeighbor.ip);Ipv4Address ip;
              }
            }
          }
        }
        else {

          oldDistance = appNode->GetNeighborDistance(sender);
          timeDif = appNode->GetNeighborInfoTime(sender) - timeNow;
          attitude = checkAttitude(oldDistance,GetDistanceFrom(tagPosition));

          if(GetDistanceFrom(tagPosition) - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 87) {
            appNode->UpdateNeighbor( sender,tagPosition,GetDistanceFrom(tagPosition), attitude,2,1,tag.GetMessageTime());
            //std::cout << timeNow << '\t' << m_nodeIP << " updated " << sender << std::endl;
            // printNeighbor(sender);
          } else 
            {appNode->UnregisterNeighbor(sender);}


          // Handle received neighbor list

          neighborhood = tag.GetNeighInfosVector();

          for(unsigned int i = 0; i < neighborhood.size(); i++) {

            aNeighbor = neighborhood.at(i);

            if(appNode->IsAlreadyNeighbor(aNeighbor.ip) ) {

              position.x = aNeighbor.x;
              position.y = aNeighbor.y;
              position.z = aNeighbor.z;
              oldDistance = appNode->GetNeighborDistance(aNeighbor.ip);
              timeDif = appNode->GetNeighborInfoTime(aNeighbor.ip) - timeNow;
              attitude = checkAttitude(oldDistance,GetDistanceFrom(position));

              if(GetDistanceFrom(position) - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 87) {
              
                appNode->UpdateNeighbor(aNeighbor.ip, position, GetDistanceFrom(position), attitude,2,
                                        compareHop(appNode->GetNeighborHop(aNeighbor.ip),aNeighbor.hop+1),aNeighbor.infoTime);
                //std::cout << timeNow << '\t' << m_nodeIP << " updated " << aNeighbor.ip << std::endl;
                // printNeighbor(aNeighbor.ip);
              } else 
                {appNode->UnregisterNeighbor(aNeighbor.ip);}
            }
            else {

              position.x = aNeighbor.x;
              position.y = aNeighbor.y;
              position.z = aNeighbor.z;

              if(aNeighbor.ip != m_nodeIP) {appNode->RegisterNeighbor( aNeighbor.ip, position, GetDistanceFrom(position), 0, 2,aNeighbor.hop+1,aNeighbor.infoTime);
                //std::cout << timeNow << '\t' << m_nodeIP << " added " << aNeighbor.ip << std::endl;
                // printNeighbor(aNeighbor.ip);
              }
            }
          }
        }

        // Create answer packet

        neighborhood.clear();

        for(unsigned int i = 0; i < neighborhoodIPList.size(); i++) {

          aNeighbor.ip = neighborhoodIPList.at(i);
          position = appNode->GetNeighborPosition(aNeighbor.ip);
          aNeighbor.x = position.x;
          aNeighbor.y = position.y;
          aNeighbor.z = position.z;
          aNeighbor.infoTime = timeNow;
          aNeighbor.hop = appNode->GetNeighborHop(aNeighbor.ip);
          neighborhood.push_back(aNeighbor);
        }

        answerTag.SetNNeighbors(0);
        answerTag.SetMessageTime(timeNow);
        answerTag.SetSimpleValue(1);
        answerTag.SetNeighInfosVector(neighborhood);
        answerTag.SetNNeighbors(neighborhoodIPList.size());

        // If node is malicious, answer broadcast with fake information

        if(!m_malicious)
          {answerTag.SetPosition(GetNodePosition());}
        else {
          Vector fakePosition(generateRandomNumber(1500),generateRandomNumber(1500),91.4);
          answerTag.SetPosition(fakePosition);
        }

        answerPacket->AddPacketTag(answerTag);

        my_socket = Socket::CreateSocket (appNode, UdpSocketFactory::GetTypeId ());
        my_socket->Bind();

        if(!my_socket->SendTo(answerPacket,0,InetSocketAddress(sender, 9)))
          {std::cout << "Error sending packet in MyPktSink broadcast";}
        
        if(!m_malicious)
          {m_myTxTrace(m_nodeIP,sender,answerTag, timeNow, GetNode());}
        else
          {m_myMaliciousTxTrace(m_nodeIP,sender,answerTag, timeNow, GetNode());}
          
        my_socket->Close();

        if(appNode->IsAlreadyMaliciousNeighbor(sender))
          {appNode->IncreaseMaliciousNeighborMsgSent(sender);}

        break;
      case 1:

        // The packet received is an answer to a broadcast. Update neighborhood only

        //std::cout << timeNow << '\t' << m_nodeIP << " received answer to broadcast from " << sender << std::endl << std::endl;
      
        if(!appNode->IsAlreadyNeighbor(sender)) {

          appNode->RegisterNeighbor(sender,tagPosition,GetDistanceFrom(tagPosition),0,2,1,tag.GetMessageTime());
          //std::cout << timeNow << '\t' << m_nodeIP << " added " << sender << std::endl;
          // printNeighbor(sender);

          // Handle received neighbor list

          neighborhood = tag.GetNeighInfosVector();

          for(unsigned int i = 0; i < neighborhood.size(); i++) {

            aNeighbor = neighborhood.at(i);

            if(aNeighbor.ip == m_nodeIP)
              {break;}

            if(appNode->IsAlreadyNeighbor(aNeighbor.ip) ) {

              position.x = aNeighbor.x;
              position.y = aNeighbor.y;
              position.z = aNeighbor.z;
              oldDistance = appNode->GetNeighborDistance(aNeighbor.ip);
              timeDif = appNode->GetNeighborInfoTime(aNeighbor.ip) - timeNow;
              attitude = checkAttitude(oldDistance,GetDistanceFrom(position));

              if(GetDistanceFrom(position) - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 87) {
                appNode->UpdateNeighbor(aNeighbor.ip, position, GetDistanceFrom(position), attitude,2,
                                        compareHop(appNode->GetNeighborHop(aNeighbor.ip),aNeighbor.hop+1),aNeighbor.infoTime);
                //std::cout << timeNow << '\t' << m_nodeIP << " updated " << aNeighbor.ip << std::endl;
                // printNeighbor(aNeighbor.ip);
              } else 
                {appNode->UnregisterNeighbor(aNeighbor.ip);}
            }
            else {

              position.x = aNeighbor.x;
              position.y = aNeighbor.y;
              position.z = aNeighbor.z;

              if(aNeighbor.ip != m_nodeIP) {
                
                appNode->RegisterNeighbor( aNeighbor.ip, position, GetDistanceFrom(position), 0, 2,aNeighbor.hop+1,aNeighbor.infoTime);
                //std::cout << timeNow << '\t' << m_nodeIP << " added " << aNeighbor.ip << std::endl;
                // printNeighbor(aNeighbor.ip);
              }
            }
          }
        }
        else {

          oldDistance = appNode->GetNeighborDistance(sender);
          timeDif = appNode->GetNeighborInfoTime(sender) - timeNow;
          attitude = checkAttitude(oldDistance,GetDistanceFrom(tagPosition));

          if(GetDistanceFrom(tagPosition) - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 87) {

            appNode->UpdateNeighbor( sender,tagPosition,GetDistanceFrom(tagPosition), attitude,2,1,tag.GetMessageTime());
            //std::cout << timeNow << '\t' << m_nodeIP << " updated " << sender << std::endl;
            // printNeighbor(sender);
          } else 
            {appNode->UnregisterNeighbor(sender);}

          

          // Handle received neighbor list

          neighborhood = tag.GetNeighInfosVector();

          for(unsigned int i = 0; i < neighborhood.size(); i++) {

            aNeighbor = neighborhood.at(i);

            if(aNeighbor.ip == m_nodeIP)
              {break;}

            if(appNode->IsAlreadyNeighbor(aNeighbor.ip) ) {

              position.x = aNeighbor.x;
              position.y = aNeighbor.y;
              position.z = aNeighbor.z;
              oldDistance = appNode->GetNeighborDistance(aNeighbor.ip);
              timeDif = appNode->GetNeighborInfoTime(aNeighbor.ip) - timeNow;
              attitude = checkAttitude(oldDistance,GetDistanceFrom(position));

              if(GetDistanceFrom(position) - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 87) {
                appNode->UpdateNeighbor( aNeighbor.ip, position, GetDistanceFrom(position),attitude,2,
                                          compareHop(appNode->GetNeighborHop(aNeighbor.ip),aNeighbor.hop+1),aNeighbor.infoTime);
                //std::cout << timeNow << '\t' << m_nodeIP << " updated " << aNeighbor.ip << std::endl;
                // printNeighbor(aNeighbor.ip);
              } else 
                {appNode->UnregisterNeighbor(aNeighbor.ip);}
            }
            else {

              position.x = aNeighbor.x;
              position.y = aNeighbor.y;
              position.z = aNeighbor.z;

              if(aNeighbor.ip != m_nodeIP) {
                appNode->RegisterNeighbor( aNeighbor.ip, position, GetDistanceFrom(position), 0, 2,aNeighbor.hop+1,aNeighbor.infoTime);
                //std::cout << timeNow << '\t' << m_nodeIP << " added " << aNeighbor.ip << std::endl;
                // printNeighbor(aNeighbor.ip);
              }
            }
          }
        }
        break;
      case 2:
          // The packet received is an update. Update neighborhood only

        //std::cout << timeNow << '\t' << m_nodeIP << " received an update from " << sender << std::endl << std::endl;
        
        if(!appNode->IsAlreadyNeighbor(sender)) {

          appNode->RegisterNeighbor(sender,tag.GetPosition(),GetDistanceFrom(tag.GetPosition()),0,2,1,tag.GetMessageTime());

          //std::cout << timeNow << '\t' << m_nodeIP << " added " << sender << std::endl;
          // printNeighbor(sender);

          // Handle received neighbor list

          neighborhood = tag.GetNeighInfosVector();

          for(unsigned int i = 0; i < neighborhood.size(); i++) {

            aNeighbor = neighborhood.at(i);

            if(aNeighbor.ip == m_nodeIP)
              {break;}

            if(appNode->IsAlreadyNeighbor(aNeighbor.ip) ) {

              position.x = aNeighbor.x;
              position.y = aNeighbor.y;
              position.z = aNeighbor.z;
              oldDistance = appNode->GetNeighborDistance(aNeighbor.ip);
              timeDif = appNode->GetNeighborInfoTime(aNeighbor.ip) - timeNow;
              attitude = checkAttitude(oldDistance,GetDistanceFrom(position));

              if(GetDistanceFrom(position) - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 87) {
                appNode->UpdateNeighbor(aNeighbor.ip, position, GetDistanceFrom(position), attitude,2,
                                        compareHop(appNode->GetNeighborHop(aNeighbor.ip),aNeighbor.hop+1),aNeighbor.infoTime);
                //std::cout << timeNow << '\t' << m_nodeIP << " updated " << aNeighbor.ip << std::endl;
                // printNeighbor(aNeighbor.ip);
              } else 
                {appNode->UnregisterNeighbor(aNeighbor.ip);}
            }
            else {

              position.x = aNeighbor.x;
              position.y = aNeighbor.y;
              position.z = aNeighbor.z;

              if(aNeighbor.ip != m_nodeIP) {
                appNode->RegisterNeighbor( aNeighbor.ip, position, GetDistanceFrom(position), 0, 2,aNeighbor.hop+1,aNeighbor.infoTime);
                //std::cout << timeNow << '\t' << m_nodeIP << " added " << aNeighbor.ip << std::endl;
                // printNeighbor(aNeighbor.ip);
              }
            }
          }
        }
        else {
          
          oldDistance = appNode->GetNeighborDistance(sender);
          timeDif = appNode->GetNeighborInfoTime(sender) - timeNow;
          attitude = checkAttitude(oldDistance,GetDistanceFrom(tagPosition));

          appNode->UpdateNeighbor( sender,tagPosition,GetDistanceFrom(tagPosition), attitude,2,1,tag.GetMessageTime());
          //std::cout << timeNow << '\t' << m_nodeIP << " updated " << sender << std::endl;
          // printNeighbor(sender);


          // Handle received neighbor list
                                    
          neighborhood = tag.GetNeighInfosVector();

          for(unsigned int i = 0; i < neighborhood.size(); i++) {

            aNeighbor = neighborhood.at(i);

            if(aNeighbor.ip == m_nodeIP)
              {break;}

            if(appNode->IsAlreadyNeighbor(aNeighbor.ip) ) {

              position.x = aNeighbor.x;
              position.y = aNeighbor.y;
              position.z = aNeighbor.z;
              oldDistance = appNode->GetNeighborDistance(aNeighbor.ip);
              timeDif = appNode->GetNeighborInfoTime(aNeighbor.ip) - timeNow;
              attitude = checkAttitude(oldDistance,GetDistanceFrom(position));

              if(GetDistanceFrom(position) - checkSpeed(oldDistance,GetDistanceFrom(position),timeDif) < 87) {
                appNode->UpdateNeighbor( aNeighbor.ip, position, GetDistanceFrom(position), attitude,2,
                                          compareHop(appNode->GetNeighborHop(aNeighbor.ip),aNeighbor.hop+1),aNeighbor.infoTime);
                //std::cout << timeNow << '\t' << m_nodeIP << " updated " << aNeighbor.ip << std::endl;
                // printNeighbor(aNeighbor.ip);
              } else 
                {appNode->UnregisterNeighbor(aNeighbor.ip);}
            }
            else {

              position.x = aNeighbor.x;
              position.y = aNeighbor.y;
              position.z = aNeighbor.z;

              if(aNeighbor.ip != m_nodeIP) {
                appNode->RegisterNeighbor( aNeighbor.ip, position, GetDistanceFrom(position), 0, 2,aNeighbor.hop+1,aNeighbor.infoTime);
                //std::cout << timeNow << '\t' << m_nodeIP << " added " << aNeighbor.ip << std::endl;
                // printNeighbor(aNeighbor.ip);
              }
            }
          }
        }
        break;

      case 3:

          // Received a malicious neighbor

        neighborhood = tag.GetNeighInfosVector();
        aNeighbor.ip = neighborhood.at(0).ip;

        std::cout << std::endl << timeNow << " " << m_nodeIP  << " Received tag 3 about " << aNeighbor.ip << std::endl;

        std::cout << std::endl;

        if(!appNode->GetMaliciousNeighborState(aNeighbor.ip) && aNeighbor.ip != m_nodeIP) {   // If node is already blocked, ignore
            
          if(appNode->IsAlreadyMaliciousNeighbor(aNeighbor.ip)) {
          
            appNode->IncreaseMaliciousNeighborRecurrence(aNeighbor.ip);
            std::cout << m_nodeIP << " increased recurrence of " << aNeighbor.ip << " to " << appNode->GetMaliciousNeighborRecurrence(aNeighbor.ip) << std::endl;
          
            if(appNode->GetMaliciousNeighborRecurrence(aNeighbor.ip) == 3) {
              appNode->SetMaliciousNeighborState(aNeighbor.ip,true);
              appNode->UnregisterNeighbor(aNeighbor.ip);
            std::cout << timeNow << " " << m_nodeIP << " blocked " << aNeighbor.ip << std::endl;
            }
          } else if(appNode->IsAlreadyNeighbor(aNeighbor.ip)) { 
          
            std::cout << m_nodeIP << " registered " << aNeighbor.ip << " on MaliciousList" << std::endl;
            appNode->SetNeighborState(aNeighbor.ip,true);
            appNode->RegisterMaliciousNeighbor(aNeighbor.ip,false,1, timeNow);

          } else {

            std::cout << m_nodeIP << " registered " << aNeighbor.ip << " on NeighborList and on MaliciousList " << std::endl;
            appNode->RegisterNeighbor(aNeighbor.ip,Vector(0,0,0),0.1,0,3,2,timeNow);
            appNode->SetNeighborState(aNeighbor.ip,true);
            appNode->RegisterMaliciousNeighbor(aNeighbor.ip,false,1, timeNow);
          }
        }

        std::cout << std::endl;

        break;

      case 4:

        // Received a blocked neighbor
        
        neighborhood = tag.GetNeighInfosVector();
        aNeighbor.ip = neighborhood.at(0).ip;

        std::cout << std::endl << timeNow << " " << m_nodeIP <<  " Received tag 4 about " << aNeighbor.ip << std::endl << std::endl;        
        std::cout << std::endl;

        
        if(appNode->IsAlreadyNeighbor(aNeighbor.ip) && aNeighbor.ip != m_nodeIP) 
          { appNode->UnregisterNeighbor(aNeighbor.ip); std::cout << m_nodeIP << " unregistered " << aNeighbor.ip << " on NeighborList" << std::endl;}

        if(!appNode->IsAlreadyMaliciousNeighbor(aNeighbor.ip) && m_nodeIP != aNeighbor.ip)
          {appNode->RegisterMaliciousNeighbor(aNeighbor.ip,true,3, timeNow);std::cout << m_nodeIP << " registered " << aNeighbor.ip << " on MaliciousList (blocked)" << std::endl;}


        std::cout << std::endl;

        // if malicious & !blocked

        break;
      
      case 5:
  
        // Received a neighbor removed from suspicious activity
        
        neighborhood = tag.GetNeighInfosVector();
        aNeighbor.ip = neighborhood.at(0).ip;

        std::cout << std::endl << timeNow << " " << m_nodeIP <<  " Received tag 5 about " << aNeighbor.ip << std::endl << std::endl;

          if(appNode->IsAlreadyMaliciousNeighbor(aNeighbor.ip) && aNeighbor.ip != m_nodeIP) {
            appNode->UnregisterMaliciousNeighbor(aNeighbor.ip);
            appNode->SetNeighborState(aNeighbor.ip,false);
            std::cout << m_nodeIP << " unregistered " << aNeighbor.ip << " on MaliciousNeighborList" << std::endl;
          }

        break;

      case 6:
        
        // Received a decreased suspicious activity
        
        neighborhood = tag.GetNeighInfosVector();
        aNeighbor.ip = neighborhood.at(0).ip;

        std::cout << std::endl << timeNow << " " << m_nodeIP <<  " Received tag 6 about " << aNeighbor.ip << std::endl << std::endl;
        
          if(appNode->IsAlreadyMaliciousNeighbor(aNeighbor.ip) && aNeighbor.ip != m_nodeIP) {
            appNode->DecreaseMaliciousNeighborRecurrence(aNeighbor.ip);
            std::cout <<  m_nodeIP << " Decreased suspicious about " << sender << std::endl << std::endl;
            if(appNode->GetMaliciousNeighborRecurrence(sender) < 1) {
              appNode->UnregisterMaliciousNeighbor(sender);
              appNode->SetNeighborState(sender,false);
              std::cout << m_nodeIP << " unregistered " << aNeighbor.ip << " on MaliciousNeighborList" << std::endl;
            }
          }

        break;

      default:
        std::cout << "Error in MyPktSink, SimpleValue not found." << std::endl;
        break;
      }
  
  if(!m_malicious)
    {m_myRxTrace(m_nodeIP, sender,InetSocketAddress::ConvertFrom(localAddress).GetIpv4 (),tag, tag.GetMessageTime(), GetNode());}
  else
    {m_myMaliciousRxTrace(m_nodeIP, sender,InetSocketAddress::ConvertFrom(localAddress).GetIpv4 (),tag, tag.GetMessageTime(), GetNode());}
  }
  
  printNeighborList();
  printMaliciousNeighborList();
    
  if (m_enableSeqTsSizeHeader)
      {
        PacketReceived (packet, from, localAddress);
      }
  }
}

void
MyPktSink::PacketReceived (const Ptr<Packet> &p, const Address &from, const Address &localAddress) {
  
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


/*
-------------------------------------
      Added Functions
-------------------------------------
*/


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

int MyPktSink::checkAttitude(double before, double after) {

  // Returns 1 if the aNeighbor is getting closer, -1 if its flying away and 0 if unchanged

  double attitude = before - after;

    if(attitude > 0)
      return 1;
    else if (attitude < 0)
      return -1;
    else
      return 0;
}

int MyPktSink::checkSpeed(double before, double after, double time) {

  // Returns 1 if the aNeighbor is getting closer, -1 if its flying away and 0 if unchanged

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

void MyPktSink::printMaliciousNeighborList() {


  Ptr<Node> n = GetNode();
  vector<Ipv4Address> neighborIPList = n->GetMaliciousNeighborIpList();

  std::cout << std::endl << m_nodeIP << " has " << neighborIPList.size() << " suspicious neighbors" << std::endl;
  for(unsigned int i = 0; i < neighborIPList.size();i++) {
    std::cout << "\tIP: " << neighborIPList.at(i) << " Recurrence: " << n->GetMaliciousNeighborRecurrence(neighborIPList.at(i))
    << " State: " << (int) n->GetMaliciousNeighborState(neighborIPList.at(i)) << " Timestamp: " << n->GetMaliciousNeighborTimestamp(neighborIPList.at(i)) << std::endl;
  }
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
  << " Timestamp: " << n->GetNeighborInfoTime(ip) << " Attitude: " << att << " Hop: " << (int) n->GetNeighborHop(ip) << " State: " << (int) n->GetNeighborState(ip) << std::endl;

}

void MyPktSink::notifyAboutMaliciousNeighbor (Ipv4Address maliciousIP, uint8_t tagType) {

  MyTag answerTag;
  MyTag::NeighInfos aNeighbor;  
  vector<MyTag::NeighInfos>neighborhood;
  Ptr<Node> appNode = GetNode();
  Ptr<Packet> answerPacket = Create<Packet> (100);
  vector<Ipv4Address> neighborsIPList = appNode->GetNeighborIpList();
  Ptr<Socket> my_socket = Socket::CreateSocket (appNode, UdpSocketFactory::GetTypeId ());

  aNeighbor.ip = maliciousIP;
  neighborhood.push_back(aNeighbor);

  answerTag.SetNNeighbors(1);
  answerTag.SetSimpleValue(tagType);
  answerTag.SetNeighInfosVector(neighborhood);
  answerTag.SetMessageTime(Simulator::Now().ToDouble(Time::S));

  my_socket->Bind();

  // if(tagType == 4)
  //   {std::cout << answerTag.GetMessageTime() << " " << m_nodeIP << " BLOCKED " << maliciousIP << std::endl << std::endl;}
  // else
  //   {std::cout << answerTag.GetMessageTime() << " " << m_nodeIP << " " << appNode->GetMaliciousNeighborRecurrence(maliciousIP) << "x suspicious about " << maliciousIP << std::endl << std::endl;}
  

  if(neighborsIPList.size() > 0) {

    for(unsigned int i = 0; i < neighborsIPList.size(); i++) {

      if(appNode->GetNeighborHop(neighborsIPList.at(i)) == 1 && !appNode->IsAlreadyMaliciousNeighbor(neighborsIPList.at(i))) {

        if(appNode->IsAlreadyMaliciousNeighbor(neighborsIPList.at(i)))
          {appNode->IncreaseMaliciousNeighborMsgSent(neighborsIPList.at(i));}          

        // Send message only to close, non-suspicious neighbors

        if(!m_malicious) {

          answerTag.SetPosition(GetNodePosition());
          answerPacket->AddPacketTag(answerTag);
        } else {
          
          Vector fakePosition(generateRandomNumber(1500),generateRandomNumber(1500),91.4);
          answerTag.SetPosition(fakePosition);
          answerPacket->AddPacketTag(answerTag);
        }

        int actual = my_socket->SendTo(answerPacket,0,InetSocketAddress(neighborsIPList.at(i), 9));
        if(!m_malicious)
          {m_myTxTrace(m_nodeIP, neighborsIPList.at(i), answerTag, answerTag.GetMessageTime(), appNode);}
        else
          {m_myMaliciousTxTrace(m_nodeIP, neighborsIPList.at(i), answerTag, answerTag.GetMessageTime(), appNode);}
        

        if(actual < 0)
          {std::cout << "Error sending packet in MyPktSink notifyAboutMaliciousNeighbor";}        
          answerPacket->RemovePacketTag(answerTag);
        }
      }
    }

  my_socket->Close();
}

int MyPktSink::generateRandomNumber(int max_value) {

  double number = rand() % max_value + 1;
  number += (rand() % 100) / 100; 

  return number;
}
} // Namespace ns3
