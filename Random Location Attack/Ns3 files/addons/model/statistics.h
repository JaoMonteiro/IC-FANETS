/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef STATISTICS_H
#define STATISTICS_H

#include "ns3/ipv4.h"
#include "ns3/vector.h"
#include "ns3/mytag.h"
#include "ns3/node-list.h"
#include "ns3/simulator.h"
#include "ns3/node-container.h"
#include "ns3/mobility-module.h"
#include <iomanip>      
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <ctime>
#include <cmath>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>


/*
--------------------------------------------
Code developed by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
--------------------------------------------
*/



namespace ns3 {

class Statistics {
  public: 

    std::string filepath;
    double m_lastUpdate;
    double m_lastEvolution;
    unsigned int m_tag0Sent;
    unsigned int m_tag0Received;
    unsigned int m_tag1Sent;
    unsigned int m_tag1Received;
    unsigned int m_tag2Sent;
    unsigned int m_tag2Received;
    unsigned int m_tag3Sent;
    unsigned int m_tag3Received;
    unsigned int m_tag4Sent;
    unsigned int m_tag4Received;
    unsigned int m_tag5Sent;
    unsigned int m_tag5Received;
    unsigned int m_tag6Sent;
    unsigned int m_tag6Received;
    unsigned int m_numberMessagesSent;
    unsigned int m_numberMessagesReceived;
  
    Statistics();
    bool fileExists(std::string file_path);
    void senderCallback(const Ipv4Address &from , const Ipv4Address &to, MyTag &tag, const double &time, const Ptr <Node>& m_node); 
    void receiverCallback(const Ipv4Address &m_nodeIP , const Ipv4Address &from , const Ipv4Address &to, MyTag &tag, const double &time, const Ptr <Node>&m_node);
    void maliciousSenderCallback(const Ipv4Address &from , const Ipv4Address &to, MyTag &tag, const double &time, const Ptr <Node>& m_node); 
    void maliciousReceiverCallback(const Ipv4Address &m_nodeIP , const Ipv4Address &from , const Ipv4Address &to, MyTag &tag, const double &time, const Ptr <Node>&m_node);
    double distanceBetweenTwoPoints(Vector a,Vector b);
    void printTotalTraces();
    void genPositionTraces();
    void genDistanceTraces();
    void genMaliciousTraces();
    double calculateDistance(Vector a, Vector b); 
    void checkNeighborhoodEvolution(const Ipv4Address &node_IP, const double &time, const Ptr<Node> & m_node);
    void checkMaliciousNeighborhoodEvolution(const Ipv4Address &node_IP, const double &time, const Ptr<Node> & m_node);
    void printNeighbor(Ipv4Address ip, Ptr<Node> m_node,std::ofstream &log);
    void printMaliciousNeighbor(Ipv4Address ip, Ptr<Node> m_node,std::ofstream &log);
    void genNeighborHoodAnalysis();
    void addMaliciousDrone(int node, double time);
    void addSimulationInfo(std::string info);
    vector< std::pair<Ipv4Address,Vector> > makePositionVector();

};

}

#endif /* STATISTICS_H */

