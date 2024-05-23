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



#ifndef MY_PACKET_SINK_H
#define MY_PACKET_SINK_H

#include "ns3/application.h"
#include "ns3/mobility-module.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/address.h"
#include "ns3/mytag.h"
#include "ns3/ipv4.h"
#include "ns3/inet-socket-address.h"
#include "ns3/seq-ts-size-header.h"
#include <unordered_map>
#include <cmath>
#include <random>

namespace ns3 {

class Address;
class Socket;
class Packet;

/**
 * \ingroup applications 
 * \defgroup packetsink MyPktSink
 *
 * This application was written to complement OnOffApplication, but it
 * is more general so a MyPktSink name was selected.  Functionally it is
 * important to use in multicast situations, so that reception of the layer-2
 * multicast frames of interest are enabled, but it is also useful for
 * unicast as an example of how you can write something simple to receive
 * packets at the application layer.  Also, if an IP stack generates 
 * ICMP Port Unreachable errors, receiving applications will be needed.
 */

/**
 * \ingroup packetsink
 *
 * \brief Receive and consume traffic generated to an IP address and port
 *
 * This application was written to complement OnOffApplication, but it
 * is more general so a MyPktSink name was selected.  Functionally it is
 * important to use in multicast situations, so that reception of the layer-2
 * multicast frames of interest are enabled, but it is also useful for
 * unicast as an example of how you can write something simple to receive
 * packets at the application layer.  Also, if an IP stack generates 
 * ICMP Port Unreachable errors, receiving applications will be needed.
 *
 * The constructor specifies the Address (IP address and port) and the 
 * transport protocol to use.   A virtual Receive () method is installed 
 * as a callback on the receiving socket.  By default, when logging is
 * enabled, it prints out the size of packets and their address.
 * A tracing source to Receive() is also available.
 */
class MyPktSink : public Application 
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  MyPktSink ();

  virtual ~MyPktSink ();

  /**
   * \return the total bytes received in this sink app
   */
  uint64_t GetTotalRx () const;

  /**
   * \return pointer to listening socket
   */
  Ptr<Socket> GetListeningSocket (void) const;

  /**
   * \return list of pointers to accepted sockets
   */
  std::list<Ptr<Socket> > GetAcceptedSockets (void) const;

  /**
   * TracedCallback signature for a reception with addresses and SeqTsSizeHeader
   *
   * \param p The packet received (without the SeqTsSize header)
   * \param from From address
   * \param to Local address
   * \param header The SeqTsSize header
   */
  typedef void (* SeqTsSizeCallback)(Ptr<const Packet> p, const Address &from, const Address & to,
                                   const SeqTsSizeHeader &header);


  // Custom Functions and attribute by Joao Monteiro D. Silva

    void Setup(Address toAddress, uint32_t protocolId, unsigned seed);

    int generateRandomNumber(int max_value);

    unsigned m_seed;
  

protected:
  virtual void DoDispose (void);
private:
  // inherited from Application base class.
  virtual void StartApplication (void);    // Called at time specified by Start
  virtual void StopApplication (void);     // Called at time specified by Stop

  /**
   * \brief Handle a packet received by the application
   * \param socket the receiving socket
   */
  void HandleRead (Ptr<Socket> socket);
  /**
   * \brief Handle an incoming connection
   * \param socket the incoming connection socket
   * \param from the address the connection is from
   */
  void HandleAccept (Ptr<Socket> socket, const Address& from);
  /**
   * \brief Handle an connection close
   * \param socket the connected socket
   */
  void HandlePeerClose (Ptr<Socket> socket);
  /**
   * \brief Handle an connection error
   * \param socket the connected socket
   */
  void HandlePeerError (Ptr<Socket> socket);

  /**
   * \brief Packet received: assemble byte stream to extract SeqTsSizeHeader
   * \param p received packet
   * \param from from address
   * \param localAddress local address
   *
   * The method assembles a received byte stream and extracts SeqTsSizeHeader
   * instances from the stream to export in a trace source.
   */
  void PacketReceived (const Ptr<Packet> &p, const Address &from, const Address &localAddress);

  /**
   * \brief Hashing for the Address class
   */
  struct AddressHash
  {
    /**
     * \brief operator ()
     * \param x the address of which calculate the hash
     * \return the hash of x
     *
     * Should this method go in address.h?
     *
     * It calculates the hash taking the uint32_t hash value of the ipv4 address.
     * It works only for InetSocketAddresses (Ipv4 version)
     */
    size_t operator() (const Address &x) const
    {
      NS_ABORT_IF (!InetSocketAddress::IsMatchingType (x));
      InetSocketAddress a = InetSocketAddress::ConvertFrom (x);
      return std::hash<uint32_t>()(a.GetIpv4 ().Get ());
    }
  };

  std::unordered_map<Address, Ptr<Packet>, AddressHash> m_buffer; //!< Buffer for received packets

  // In the case of TCP, each socket accept returns a new socket, so the
  // listening socket is stored separately from the accepted sockets
  Ptr<Socket>     m_socket;       //!< Listening socket
  std::list<Ptr<Socket> > m_socketList; //!< the accepted sockets

  Address         m_local;        //!< Local address to bind to
  uint64_t        m_totalRx;      //!< Total bytes received
  TypeId          m_tid;          //!< Protocol TypeId

  bool            m_enableSeqTsSizeHeader {false}; //!< Enable or disable the export of SeqTsSize header 

  /// Traced Callback: received packets, source address.
  TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;

  // Custom Callbacks by Joao Monteiro D. Silva
    /// Traced Callback: received packets.
    TracedCallback<const Ipv4Address &, const Ipv4Address &, const Ipv4Address &, MyTag &, const double &, const double &, const Ptr <ns3::Node>& > m_myRxTrace;
    /// Traced Callback: transmitted packets.
    TracedCallback<const Ipv4Address &, const Ipv4Address &, MyTag &, const double &, const Ptr <Node>&> m_myTxTrace;

  /// Callback for tracing the packet Rx events, includes source and destination addresses
  TracedCallback<Ptr<const Packet>, const Address &, const Address &> m_rxTraceWithAddresses;
  /// Callbacks for tracing the packet Rx events, includes source, destination addresses, and headers
  TracedCallback<Ptr<const Packet>, const Address &, const Address &, const SeqTsSizeHeader&> m_rxTraceWithSeqTsSize;


  // Custom attributes by Joao Monteiro D. Silva
    Address   m_node;
    Ipv4Address m_nodeIP;

  // Custom Functions by Joao Monteiro D. Silva

    Address GetNodeIpAddress();
    
    Vector GetNodePosition();
    
    double GetDistanceFrom(Vector point);
    
    int checkAttitude(double before, double after);
    
    int checkSpeed(double before, double after,double time);
    
    uint8_t compareHop(uint8_t a, uint8_t b);
    
    void printNeighbor(Ipv4Address ip);
    
    void printNeighborList();

};

} // namespace ns3

#endif /* MY_PACKET_SINK_H */

