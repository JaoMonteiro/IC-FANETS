#ifndef MYTAG_H
#define MYTAG_H

#include "ns3/tag.h"
#include "ns3/uinteger.h"
#include "ns3/vector.h"
#include "ns3/ipv4.h"

/*
--------------------------------------------
Code edited by Agnaldo Batista UFPR <agnaldosb@gmail.com> and João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
--------------------------------------------
*/


using namespace std;

namespace ns3 {

class MyTag : public Tag {
public:
  static TypeId GetTypeId(void);
  virtual TypeId GetInstanceTypeId(void) const;
  virtual uint32_t GetSerializedSize(void) const;
  virtual void Serialize(TagBuffer i) const;
  virtual void Deserialize(TagBuffer i);
  virtual void Print(ostream &os) const;
  void SetSimpleValue(uint8_t value);
  uint8_t GetSimpleValue(void) const;

  //These are custom accessor
	Vector GetPosition(void);                     //!< Get nodes position
	void SetPosition (Vector pos);                //!< Set nodes position
	uint32_t GetNNeighbors(void);                 //!< Get the number of neighbor nodes
	void SetNNeighbors (uint32_t nNeighbors);     //!< Set the number of neighbor nodes
  void SetMessageTime(double time);             //!< Set message sent time to tag
  double GetMessageTime();                      //!< Get message sent time to tag

  /**
   * @brief Struct to store infos from a neighbor node
   * @date 19012023
   */
  struct NeighInfos {
        Ipv4Address ip;
        double x;
        double y;
        double z;
        uint8_t hop;
        double infoTime;
  };

  /**
   * \brief Neighbor entry.
   * This structure is used to store Neighbors' node information
   * @date 06042023
   */

  struct NeighborFull {
        Ipv4Address ip; //!< the neighbor IP address
        Vector position;
        double distance;
        uint8_t attitude;
        uint8_t quality;
        uint8_t hop; //!< hop = 1 means neighbor is in range
        double infoTime; 
  }; 

  vector<NeighInfos> GetNeighInfosVector() const;
	void SetNeighInfosVector(const vector<NeighInfos> neighInfosVector);

private:
  uint8_t m_simpleValue;                  //!< Tag value
  uint32_t m_nNeighborsValue;             //!< Number of neighbor nodes
	Vector m_currentPosition;               //!< Current position
  vector<NeighInfos> m_neighInfosVector;  //!< Store a list of neighbor nodes infos
  double m_messageTime;                   //!< Store message sent time
};
} // namespace ns3

#endif
