/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006 Georgia Tech Research Corporation, INRIA
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
 * Authors: George F. Riley<riley@ece.gatech.edu>
 *          Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

/*
--------------------------------------------
Code edited by Agnaldo Batista UFPR <agnaldosb@gmail.com> and João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
--------------------------------------------
*/

#ifndef NODE_H
#define NODE_H

#include <vector>

#include "ns3/object.h"
#include "ns3/callback.h"
#include "ns3/ptr.h"
#include "ns3/net-device.h"
#include "ns3/vector.h"
#include <list>
#include <iostream>

namespace ns3 {

class Application;
class Packet;
class Address;
class Time;


/**
 * \ingroup network
 *
 * \brief A network Node.
 *
 * This class holds together:
 *   - a list of NetDevice objects which represent the network interfaces
 *     of this node which are connected to other Node instances through
 *     Channel instances.
 *   - a list of Application objects which represent the userspace
 *     traffic generation applications which interact with the Node
 *     through the Socket API.
 *   - a node Id: a unique per-node identifier.
 *   - a system Id: a unique Id used for parallel simulations.
 *
 * Every Node created is added to the NodeList automatically.
 */
class Node : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  Node();
  /**
   * \param systemId a unique integer used for parallel simulations.
   */
  Node(uint32_t systemId);

  virtual ~Node();

  /**
   * \returns the unique id of this node.
   * 
   * This unique id happens to be also the index of the Node into
   * the NodeList. 
   */
  uint32_t GetId (void) const;

  /**
   * In the future, ns3 nodes may have clock that returned a local time
   * different from the virtual time Simulator::Now().
   * This function is currently a placeholder to ease the development of this feature.
   * For now, it is only an alias to Simulator::Now()
   *
   * \return The time as seen by this node
   */
  Time GetLocalTime (void) const;

  /**
   * \returns the system id for parallel simulations associated
   *          to this node.
   */
  uint32_t GetSystemId (void) const;

  /**
   * \brief Associate a NetDevice to this node.
   *
   * \param device NetDevice to associate to this node.
   * \returns the index of the NetDevice into the Node's list of
   *          NetDevice.
   */
  uint32_t AddDevice (Ptr<NetDevice> device);
  /**
   * \brief Retrieve the index-th NetDevice associated to this node.
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  Ptr<NetDevice> GetDevice (uint32_t index) const;
  /**
   * \returns the number of NetDevice instances associated
   *          to this Node.
   */
  uint32_t GetNDevices (void) const;

  /**
   * \brief Associate an Application to this Node.
   *
   * \param application Application to associate to this node.
   * \returns the index of the Application within the Node's list
   *          of Application.
   */
  uint32_t AddApplication (Ptr<Application> application);
  /**
   * \brief Retrieve the index-th Application associated to this node.
   *
   * \param index the index of the requested Application
   * \returns the requested Application.
   */
  Ptr<Application> GetApplication (uint32_t index) const;

  /**
   * \returns the number of Application instances associated to this Node.
   */
  uint32_t GetNApplications (void) const;

  /**
   * A protocol handler
   *
   * \param device a pointer to the net device which received the packet
   * \param packet the packet received
   * \param protocol the 16 bit protocol number associated with this packet.
   *        This protocol number is expected to be the same protocol number
   *        given to the Send method by the user on the sender side.
   * \param sender the address of the sender
   * \param receiver the address of the receiver; Note: this value is
   *                 only valid for promiscuous mode protocol
   *                 handlers.  Note:  If the L2 protocol does not use L2
   *                 addresses, the address reported here is the value of 
   *                 device->GetAddress().
   * \param packetType type of packet received
   *                   (broadcast/multicast/unicast/otherhost); Note:
   *                   this value is only valid for promiscuous mode
   *                   protocol handlers.
   */
  typedef Callback<void,Ptr<NetDevice>, Ptr<const Packet>,uint16_t,const Address &,
                   const Address &, NetDevice::PacketType> ProtocolHandler;
  /**
   * \param handler the handler to register
   * \param protocolType the type of protocol this handler is 
   *        interested in. This protocol type is a so-called
   *        EtherType, as registered here:
   *        http://standards.ieee.org/regauth/ethertype/eth.txt
   *        the value zero is interpreted as matching all
   *        protocols.
   * \param device the device attached to this handler. If the
   *        value is zero, the handler is attached to all
   *        devices on this node.
   * \param promiscuous whether to register a promiscuous mode handler
   */
  void RegisterProtocolHandler (ProtocolHandler handler, 
                                uint16_t protocolType,
                                Ptr<NetDevice> device,
                                bool promiscuous=false);
  /**
   * \param handler the handler to unregister
   *
   * After this call returns, the input handler will never
   * be invoked anymore.
   */
  void UnregisterProtocolHandler (ProtocolHandler handler);

  /**
   * A callback invoked whenever a device is added to a node.
   */
  typedef Callback<void,Ptr<NetDevice> > DeviceAdditionListener;
  /**
   * \param listener the listener to add
   *
   * Add a new listener to the list of listeners for the device-added
   * event. When a new listener is added, it is notified of the existence
   * of all already-added devices to make discovery of devices easier.
   */
  void RegisterDeviceAdditionListener (DeviceAdditionListener listener);
  /**
   * \param listener the listener to remove
   *
   * Remove an existing listener from the list of listeners for the 
   * device-added event.
   */
  void UnregisterDeviceAdditionListener (DeviceAdditionListener listener);



  /**
   * \returns true if checksums are enabled, false otherwise.
   */
  static bool ChecksumEnabled (void);

 
 /* 
  
  Modifications made by Agnaldo de Souza Batista <asbatista@inf.ufpr.br> and Joao Monteiro Delveaux Silva <joaomont@ufmg.com.br>
  - Paper "Serviços Confiaveis em Redes de UAVs"
  - Create a list to store node's neighbors and methods to access it
  - Insert new node's attributes
  - Create a malicious neighbor list and methods to access it
  - Create a "State" in which this node makes malicious actions

*/

  void RegisterNeighbor (Ipv4Address ip, Vector position, double distance, u_int8_t attitude, u_int8_t quality, uint8_t hop, double time);
  void UnregisterNeighbor (Ipv4Address ip);
  void UpdateNeighbor (Ipv4Address ip, Vector position, double distance, uint8_t attitude, uint8_t quality, uint8_t hop, double time);
  
  Vector GetNeighborPosition (Ipv4Address ip);
  double GetNeighborDistance (Ipv4Address ip);
  uint8_t GetNeighborAttitude (Ipv4Address ip);
  void SetNeighborAttitude (Ipv4Address ip, uint8_t attitude);
  uint8_t GetNeighborQuality (Ipv4Address ip);
  void SetNeighborQuality (Ipv4Address ip, uint8_t quality);
  uint8_t GetNeighborHop (Ipv4Address ip);
  void SetNeighborHop (Ipv4Address ip, uint8_t hop);
  double GetNeighborInfoTime (Ipv4Address ip);
  void SetNeighborInfoTime (Ipv4Address ip, double time);

  bool IsThereAnyNeighbor ();
  bool IsThereAnyNeighbor (uint8_t hop);
  std::vector<Ipv4Address> GetNeighborIpList ();
  void TurnOffLiveNeighbors ();
  bool IsAlreadyNeighbor (Ipv4Address ip);
  int GetNNeighbors ();
  void ClearNeighborList ();
  Vector GetPosition (void);
  bool isAttending (void);
  bool isMoving (Vector position);
  void SetPosition (Vector position);
  Address GetNodeIpAddress();

protected:
  /**
   * The dispose method. Subclasses must override this method
   * and must chain up to it by calling Node::DoDispose at the
   * end of their own DoDispose method.
   */
  virtual void DoDispose (void);
  virtual void DoInitialize (void);
private:

  /**
   * \brief Notifies all the DeviceAdditionListener about the new device added.
   * \param device the added device to notify.
   */
  void NotifyDeviceAdded (Ptr<NetDevice> device);

  /**
   * \brief Receive a packet from a device in non-promiscuous mode.
   * \param device the device
   * \param packet the packet
   * \param protocol the protocol
   * \param from the sender
   * \returns true if the packet has been delivered to a protocol handler.
   */
  bool NonPromiscReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol, const Address &from);
  /**
   * \brief Receive a packet from a device in promiscuous mode.
   * \param device the device
   * \param packet the packet
   * \param protocol the protocol
   * \param from the sender
   * \param to the destination
   * \param packetType the packet type
   * \returns true if the packet has been delivered to a protocol handler.
   */
  bool PromiscReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol,
                                 const Address &from, const Address &to, NetDevice::PacketType packetType);
  /**
   * \brief Receive a packet from a device.
   * \param device the device
   * \param packet the packet
   * \param protocol the protocol
   * \param from the sender
   * \param to the destination
   * \param packetType the packet type
   * \param promisc true if received in promiscuous mode
   * \returns true if the packet has been delivered to a protocol handler.
   */
  bool ReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet>, uint16_t protocol,
                          const Address &from, const Address &to, NetDevice::PacketType packetType, bool promisc);

  /**
   * \brief Finish node's construction by setting the correct node ID.
   */
  void Construct (void);

  /**
   * \brief Protocol handler entry.
   * This structure is used to demultiplex all the protocols.
   */
  struct ProtocolHandlerEntry {
    ProtocolHandler handler; //!< the protocol handler
    Ptr<NetDevice> device;   //!< the NetDevice
    uint16_t protocol;       //!< the protocol number
    bool promiscuous;        //!< true if it is a promiscuous handler
  };

  /// Typedef for protocol handlers container
  typedef std::vector<struct Node::ProtocolHandlerEntry> ProtocolHandlerList;
  /// Typedef for NetDevice addition listeners container
  typedef std::vector<DeviceAdditionListener> DeviceAdditionListenerList;

  uint32_t    m_id;         //!< Node id for this node
  uint32_t    m_sid;        //!< System id for this node
  std::vector<Ptr<NetDevice> > m_devices; //!< Devices associated to this node
  std::vector<Ptr<Application> > m_applications; //!< Applications associated to this node
  ProtocolHandlerList m_handlers; //!< Protocol handlers in the node
  DeviceAdditionListenerList m_deviceAdditionListeners; //!< Device addition listeners in the node


  /**
   * \brief Neighbor entry.
   * This structure is used to store Neighbors' node.
   */

  struct Neighbor
  {
    Ipv4Address ip;   
    Vector position;
    double distance;
    uint8_t attitude;
    uint8_t quality;
    uint8_t hop;      // Hop = 1 means neighbor in range
    double infoTime;
    std::list<Ipv4Address> possibleNeighbors;
  }; 

  struct MaliciousNeighbor
  {
    Ipv4Address ip;
    bool state;       // 0 if Suspicious, 1 if blocked
    int recurrence;   // Value indicating how suspicious a neighbor is
    double timestamp;  // Indicates when node was added as malicious 
    double timeToBlock;  // Indicates in how much time the node was blocked
    int msgSent;
    int msgReceived;
  }; 

  // Typedef for neighbors handlers container
  typedef std::vector<struct Node::Neighbor> NeighborHandlerList;
  typedef std::vector<struct Node::MaliciousNeighbor> MaliciousNeighborHandlerList;
  NeighborHandlerList m_neighborList; //!< Neighbor list in the node
  MaliciousNeighborHandlerList m_maliciousNeighborList; //!< Neighbor list in the node
  Vector m_position; //!< Store node position
  bool m_state;
  std::list<Ipv4Address> m_maliciousNeighborsSenders;
};


} // namespace ns3

#endif /* NODE_H */
