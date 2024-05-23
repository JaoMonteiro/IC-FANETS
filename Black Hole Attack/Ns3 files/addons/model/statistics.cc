#include "statistics.h"

/*
--------------------------------------------
Code developed by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
--------------------------------------------
*/


namespace ns3 {

Statistics::Statistics () {

  m_tag0Sent = 0;
  m_tag0Received = 0;
  m_tag1Sent = 0;
  m_tag1Received = 0;
  m_tag2Sent = 0;
  m_tag2Received = 0;
  m_tag3Sent = 0;
  m_tag3Received = 0;
  m_tag4Sent = 0;
  m_tag4Received = 0;
  m_tag5Sent = 0;
  m_tag5Received = 0;
  m_tag6Sent = 0;
  m_tag6Received = 0;
  m_numberMessagesReceived = 0;
  m_numberMessagesSent = 0;
  m_lastUpdate = 0;
  m_lastEvolution = 0;

}

bool Statistics::fileExists(std::string file_path) {

  FILE * test;
  test = fopen(file_path.c_str(),"r");

  if(test)
    {fclose(test);return true;}
  else
    {return false;}

}

void Statistics::addMaliciousDrone(int node, double time) {

  std::ofstream logFile;
  std::stringstream filename;

  filename << filepath << "/MaliciousDrones.txt";

  // Create the trace file header

  if(!fileExists(filename.str())) {

    logFile.open(filename.str());

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.senderCallback - Malicious" << std::endl;}
      else 
        { logFile << "TurnedMaliciousOnTime\tNode" << std::endl;}

    logFile.close();
  }


  // Append information on trace file

  logFile.open(filename.str(),std::ios_base::app);

if(!logFile.is_open())
    {std::cout << "Error trying to open file in statistics.senderCallback - Malicious" << std::endl;}
  else {

    logFile << time << '\t' << "1.1.1." << node + 1 << std::endl;
    logFile.close();
  }
}

void Statistics::senderCallback(const Ipv4Address &from , const Ipv4Address &to, MyTag &tag, const double &time, const Ptr <Node>& m_node) {

  if(time - m_lastUpdate > 0.0001)
    { m_lastUpdate = time; genPositionTraces(); genDistanceTraces(); checkNeighborhoodEvolution(from,time,m_node);}

  std::ofstream logFile;
  std::stringstream iphelper;
  std::stringstream filename;
  std::stringstream filename2;


  Vector position = tag.GetPosition() ;

  iphelper << from;
  std::string ipWithoutDots(iphelper.str());

  for(unsigned int i = 0; i < ipWithoutDots.size(); i++)  {
      if(ipWithoutDots[i] == '.')
        {ipWithoutDots[i] = '_';} 
  }



  switch (tag.GetSimpleValue())
  {
  case 0:

      //-------------------------------------
      // The message sent is a broadcast
      //-------------------------------------
    
    m_tag0Sent++;
    m_numberMessagesSent++;

    filename << filepath << "/BroadCasted_" << ipWithoutDots << ".txt";


      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - BroadCast" << std::endl;}
        else 
          { logFile << "Time\tPosition\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

      logFile.close();
    }


    // Append information on trace file
  
  logFile.open(filename.str(),std::ios_base::app);

  if(!logFile.is_open())
      {std::cout << "Error trying to open file in statistics.senderCallback - BroadCast" << std::endl;}
    else {

      logFile << time << '\t' << position.x << "," << position.y << "," << position.z << '\t' << tag.GetNNeighbors() << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();

        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
          logFile << neighbourhood.at(i).ip << ",";
          Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
          logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
        }

      logFile << std::endl;
      logFile.close();
      
    }

  // Append information on trace file


    filename2 << filepath << "/SentMessages_" << ipWithoutDots << ".txt";
    
      // Create the trace file header

    if(!fileExists(filename2.str())) {

      logFile.open(filename2.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - BroadCast Answer" << std::endl;}
        else 
          { logFile << "Time\tDestination\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

      logFile.close();
    }

    logFile.open(filename2.str(),std::ios_base::app);


    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.senderCallback - BroadCast" << std::endl;}
      else 
        { 
          logFile << time << '\t' << to << '\t' << position.x << "," << position.y 
          << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
          
          vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
          for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
            logFile << neighbourhood.at(i).ip << ",";
            Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
            logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
          }
          logFile << std::endl;
          logFile.close();
        }

    break;

  case 1:

      //-------------------------------------
      // The message sent is an answer to a broadcast
      //-------------------------------------

    m_tag1Sent++;
    m_numberMessagesSent++;

    filename << filepath << "/SentMessages_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - BroadCast Answer" << std::endl;}
        else 
          { logFile << "Time\tDestination\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

      logFile.close();
    }

    logFile.open(filename.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.senderCallback - BroadCast Answer" << std::endl;}
      else 
        { 
          logFile << time << '\t' << to << '\t' << position.x << "," << position.y 
          << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
          
          vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
          for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
            logFile << neighbourhood.at(i).ip << ",";
            Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
            logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
          }
          logFile << std::endl;
          logFile.close();
        }

    break;

  case 2:

      //-------------------------------------
      // The message sent is an update
      //-------------------------------------

    m_tag2Sent++;
    m_numberMessagesSent++;

    filename << filepath << "/SentMessages_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - Update" << std::endl;}
        else 
          { logFile << "Time\tDestination\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

      logFile.close();
    }

    logFile.open(filename.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.senderCallback - Update" << std::endl;}
      else 
        { 
          logFile << time << '\t' << to << '\t' << position.x << "," << position.y 
          << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
          
          vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
          for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
            logFile << neighbourhood.at(i).ip << ",";
            Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
            logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
          }
          logFile << std::endl;
          logFile.close();
        }

    break;
  case 3:

      //-------------------------------------
      // The message sent is an update of a suspicious neighbor
      //-------------------------------------

    m_tag3Sent++;
    m_numberMessagesSent++;

    filename << filepath << "/SentMessages_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - Suspicious Neighbor" << std::endl;}
        else 
          { logFile << "Time\tDestination\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop/Recurrence)" << std::endl;}

      logFile.close();
    }

    logFile.open(filename.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.senderCallback - Suspicious Neighbor" << std::endl;}
      else 
        { 
          logFile << time << '\t' << to << '\t' << position.x << "," << position.y 
          << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
          
          vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
          for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
            logFile << neighbourhood.at(i).ip << ",";
            Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
            logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
          }
          logFile << std::endl;
          logFile.close();
        }


      filename2 << filepath << "/WarningMessagesSent_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename2.str())) {

      logFile.open(filename2.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - Suspicious" << std::endl;}
        else 
          { logFile << "Time\tDestination\tTagType\tBlockedState\tMaliciousNeighbor NeighborSenders" << std::endl;}

      logFile.close();
    }

    logFile.open(filename2.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.senderCallback - Suspicious" << std::endl;}
      else 
        { 
          logFile << time << '\t' << to << '\t' << (int) tag.GetSimpleValue() << '\t' << '0' << '\t';
          
          vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
          for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
            logFile << neighbourhood.at(i).ip << '\t';
          }
          logFile << std::endl;
          logFile.close();
        }
  
    break;
  case 4:

      //-------------------------------------
      // The message sent is an update of a suspicious neighbor
      //-------------------------------------

    m_tag4Sent++;
    m_numberMessagesSent++;

    filename << filepath << "/SentMessages_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - Suspicious Neighbor" << std::endl;}
        else 
          { logFile << "Time\tDestination\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop/Recurrence)" << std::endl;}

      logFile.close();
    }

    logFile.open(filename.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.senderCallback - Suspicious Neighbor" << std::endl;}
      else 
        { 
          logFile << time << '\t' << to << '\t' << position.x << "," << position.y 
          << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
          
          vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
          for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
            logFile << neighbourhood.at(i).ip << ",";
            Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
            logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
          }
          logFile << std::endl;
          logFile.close();
        }


      filename2 << filepath << "/WarningMessagesSent_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename2.str())) {

      logFile.open(filename2.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - removed Suspicious" << std::endl;}
        else 
          { logFile << "Time\tDestination\tTagType\tBlockedState\tMaliciousNeighbor NeighborSenders" << std::endl;}

      logFile.close();
    }

    logFile.open(filename2.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.senderCallback - removed Suspicious" << std::endl;}
      else 
        { 
          logFile << time << '\t' << to << '\t' << (int) tag.GetSimpleValue() << '\t' << '0' << '\t';
          
          vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
          for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
            logFile << neighbourhood.at(i).ip << '\t';
          }
          logFile << std::endl;
          logFile.close();
        }
  
    break;
    case 5:
    
      //-------------------------------------
      // The message sent is a request to check a suspicious neighbor
      //-------------------------------------

    m_tag5Sent++;
    m_numberMessagesSent++;

    filename << filepath << "/SentMessages_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - Tag 5" << std::endl;}
        else 
          { logFile << "Time\tDestination\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop/Recurrence)" << std::endl;}

      logFile.close();
    }

    logFile.open(filename.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.senderCallback - Tag 5" << std::endl;}
      else 
        { 
          logFile << time << '\t' << to << '\t' << position.x << "," << position.y 
          << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
          
          vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
          for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
            logFile << neighbourhood.at(i).ip << ",";
            Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
            logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
          }
          logFile << std::endl;
          logFile.close();
        }


      filename2 << filepath << "/WarningMessagesSent_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename2.str())) {

      logFile.open(filename2.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - removed Suspicious" << std::endl;}
        else 
          { logFile << "Time\tDestination\tTagType\tBlockedState\tMaliciousNeighbor NeighborSenders" << std::endl;}

      logFile.close();
    }

    logFile.open(filename2.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.senderCallback - removed Suspicious" << std::endl;}
      else 
        { 
          logFile << time << '\t' << to << '\t' << (int) tag.GetSimpleValue() << '\t' << '0' << '\t';
          
          vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
          for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
            logFile << neighbourhood.at(i).ip << '\t';
          }
          logFile << std::endl;
          logFile.close();
        }
  
    break;
    case 6:

      //-------------------------------------
      // The message sent is an answer to check a neighbor
      //-------------------------------------

    m_tag6Sent++;
    m_numberMessagesSent++;

    filename << filepath << "/SentMessages_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - Tag 6" << std::endl;}
        else 
          { logFile << "Time\tDestination\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop/Recurrence)" << std::endl;}

      logFile.close();
    }

    logFile.open(filename.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.senderCallback - Tag 6" << std::endl;}
      else 
        { 
          logFile << time << '\t' << to << '\t' << position.x << "," << position.y 
          << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
          
          vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
          for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
            logFile << neighbourhood.at(i).ip << ",";
            Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
            logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
          }
          logFile << std::endl;
          logFile.close();
        }


      filename2 << filepath << "/WarningMessagesSent_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename2.str())) {

      logFile.open(filename2.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - removed Suspicious" << std::endl;}
        else 
          { logFile << "Time\tDestination\tTagType\tBlockedState\tMaliciousNeighbor NeighborSenders" << std::endl;}

      logFile.close();
    }

    logFile.open(filename2.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.senderCallback - removed Suspicious" << std::endl;}
      else 
        { 
          logFile << time << '\t' << to << '\t' << (int) tag.GetSimpleValue() << '\t' << '0' << '\t';
          
          vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
          for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
            logFile << neighbourhood.at(i).ip << '\t';
          }
          logFile << std::endl;
          logFile.close();
        }
  
    break;
  default:
    std::cout << "Failed to read SimpleValue in statistics.senderCallback" << std::endl;
    break;
    
  }

    // Save information on the main tracefile

  std::stringstream globalFilename;
  globalFilename << filepath << "/AllMessages.txt";


    // Create the main trace file header

  if(!fileExists(globalFilename.str())) {


    logFile.open(globalFilename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - Global" << std::endl;}
        else 
          { logFile << "Time\tSender\tReceiver\tAction\tTagType\tPosition\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

      logFile.close();
    }
  
  
    // Append information on main trace file

  logFile.open(globalFilename.str(),std::ios_base::app);

  if(!logFile.is_open())
      {std::cout << "Error trying to open file in statistics.senderCallback - Global" << std::endl;}
    
    else {
       logFile << time << '\t' << from << '\t' << to << '\t' << (int) tag.GetSimpleValue() << '\t'
                << position.x << "," << position.y << "," << position.z << '\t' << tag.GetNNeighbors() << '\t';
      
        vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
          logFile << neighbourhood.at(i).ip << ",";
          Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
          logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
        }
    
      logFile << std::endl;
      logFile.close();
    }

}

void Statistics::receiverCallback(const Ipv4Address &m_nodeIP , const Ipv4Address &from , const Ipv4Address &to, MyTag &tag, const double &time, const double &latency, const Ptr <Node>& m_node) {


  m_lastUpdate = time; 
  genPositionTraces();
  genDistanceTraces();
  checkNeighborhoodEvolution(m_nodeIP,time,m_node);
  checkMaliciousNeighborhoodEvolution(m_nodeIP,time,m_node);
  
  std::stringstream iphelper;
  std::stringstream filename;
  std::stringstream filename2;

  Vector position = tag.GetPosition() ;
  
  iphelper << m_nodeIP;
  std::string ipWithoutDots(iphelper.str());

  for(unsigned int i = 0; i < ipWithoutDots.size(); i++)  {
      if(ipWithoutDots[i] == '.')
        {ipWithoutDots[i] = '_';} 
  }

std::ofstream logFile;

switch (tag.GetSimpleValue())
{
case 0:

    //-------------------------------------
    // The message received is a broadcast
    //-------------------------------------

  m_tag0Received++;
  m_numberMessagesReceived++;

  filename << filepath << "/BroadCastReceived_" << ipWithoutDots << ".txt";


    // Create header on trace file

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.receiverCallback - BroadCast" << std::endl;}
        else 
          { logFile << "Time\tLatency\tFrom\tPosition\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

      logFile.close();
    }
  
  logFile.open(filename.str(),std::ios_base::app);

  if(!logFile.is_open())
      {std::cout << "Error trying to open file in statistics.receiverCallback - BroadCast" << std::endl;}
    else 
      {
      logFile << time << '\t' << latency << '\t' << from << '\t' << position.x << "," << position.y << "," << position.z << '\t' << tag.GetNNeighbors() << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
          logFile << neighbourhood.at(i).ip << ",";
          Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
          logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
        }
      logFile << std::endl;
      
    }

  logFile.close();


    // Append information on trace file

  filename2 << filepath << "/ReceivedMessages_" << ipWithoutDots << ".txt";

  // Create header on trace file

    if(!fileExists(filename2.str())) {

      logFile.open(filename2.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.receiverCallback - BroadCast" << std::endl;}
        else 
          { logFile << "Time\tLatency\tFrom\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

      logFile.close();
    }

  logFile.open(filename2.str(),std::ios_base::app);

  if(!logFile.is_open())
      {std::cout << "Error trying to open file in statistics.receiverCallback - BroadCast" << std::endl;}
    else 
      {
      logFile << time << '\t' << latency << '\t' << from <<'\t' << position.x << "," 
      << position.y << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
          logFile << neighbourhood.at(i).ip << ",";
          Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
          logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
        }
      logFile << std::endl;
      
    }

  logFile.close();

  break;

  case 1:

  //-------------------------------------
  // The message received is an answer to a broadcast
  //-------------------------------------

  m_tag1Received++;
  m_numberMessagesReceived++;

  filename << filepath << "/ReceivedMessages_" << ipWithoutDots << ".txt";


    // Create header on trace file

  if(!fileExists(filename.str())) {

    logFile.open(filename.str());

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.receiverCallback - BroadCast Answer" << std::endl;}
      else 
        { logFile << "Time\tLatency\tFrom\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

    logFile.close();
  }
  

    // Append information on trace file

  logFile.open(filename.str(),std::ios_base::app);

  if(!logFile.is_open())
      {std::cout << "Error trying to open file in statistics.receiverCallback - BroadCast Answer" << std::endl;}
    else 
      {
      logFile << time << '\t' << latency << '\t' << from <<'\t' << position.x << "," 
      << position.y << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
          logFile << neighbourhood.at(i).ip << ",";
          Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
          logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
        }
      logFile << std::endl;
      
    }

  logFile.close();
  break;

  case 2:

  //-------------------------------------
  // The message received is an update
  //-------------------------------------

  m_tag2Received++;
  m_numberMessagesReceived++;

  filename << filepath << "/ReceivedMessages_" << ipWithoutDots << ".txt";


    // Create header on trace file

  if(!fileExists(filename.str())) {

    logFile.open(filename.str());

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.receiverCallback - Update" << std::endl;}
      else 
        { logFile << "Time\tLatency\tFrom\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

    logFile.close();
  }
  

    // Append information on trace file

  logFile.open(filename.str(),std::ios_base::app);

  if(!logFile.is_open())
      {std::cout << "Error trying to open file in statistics.receiverCallback - Update" << std::endl;}
    else 
      {
      logFile << time << '\t' << latency << '\t' << from <<'\t' << position.x << "," 
      << position.y << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
          logFile << neighbourhood.at(i).ip << ",";
          Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
          logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
        }
      logFile << std::endl;
      
    }

  logFile.close();
    break;
  case 3:

      //-------------------------------------
      // The message received is an update of a suspicious neighbor
      //-------------------------------------

    m_tag3Received++;
    m_numberMessagesReceived++;

    filename << filepath << "/ReceivedMessages_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.receivercallback - Suspicious Neighbor" << std::endl;}
        else 
          { logFile << "Time\tLatency\tFrom\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

      logFile.close();
    }

    logFile.open(filename.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.receivercallback - Suspicious Neighbor" << std::endl;}
      else 
        { 
          logFile << time << '\t' << latency << '\t' << from <<'\t' << position.x << "," 
      << position.y << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
          logFile << neighbourhood.at(i).ip << ",";
          Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
          logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
        }
      logFile << std::endl;
      }

      
      logFile.close();

      filename2 << filepath << "/WarningMessagesReceived_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename2.str())) {

      logFile.open(filename2.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.receivercallback - Suspicious Neighbor" << std::endl;}
        else 
          { logFile << "Time\tFrom\tPosition\tTagType\tBlockedState\tNeighbor" << std::endl;}

      logFile.close();
    }


    logFile.open(filename2.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.receivercallback - Suspicious Neighbor" << std::endl;}
      else 
        { 
          logFile << time << '\t' << from <<'\t' << position.x << "," 
      << position.y << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << '0' << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) 
          {logFile << neighbourhood.at(i).ip;}
      logFile << std::endl;
      }
  
    break;
    case 4:

      //-------------------------------------
      // The message received is an clearance of a suspicious neighbor
      //-------------------------------------

    m_tag4Received++;
    m_numberMessagesReceived++;

    filename << filepath << "/ReceivedMessages_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.receivercallback - Suspicious Neighbor" << std::endl;}
        else 
          { logFile << "Time\tLatency\tFrom\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

      logFile.close();
    }

    logFile.open(filename.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.receivercallback - Suspicious Neighbor" << std::endl;}
      else 
        { 
          logFile << time << '\t' << latency << '\t' << from <<'\t' << position.x << "," 
      << position.y << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
          logFile << neighbourhood.at(i).ip << ",";
          Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
          logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
        }
      logFile << std::endl;
      }

      
      logFile.close();

      filename2 << filepath << "/WarningMessagesReceived_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename2.str())) {

      logFile.open(filename2.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.receivercallback - removed Suspicious Neighbor" << std::endl;}
        else 
          { logFile << "Time\tFrom\tPosition\tTagType\tBlockedState\tMaliciousNeighbor NeighborSenders" << std::endl;}

      logFile.close();
    }


    logFile.open(filename2.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.receivercallback - removed Suspicious Neighbor" << std::endl;}
      else 
        { 
          logFile << time << '\t' << from <<'\t' << position.x << "," 
      << position.y << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << '0' << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) 
          {logFile << neighbourhood.at(i).ip;}
      logFile << std::endl;
      }
  
    break;
    case 5:

      //-------------------------------------
      // The message received is an clearance of a suspicious neighbor
      //-------------------------------------

    m_tag5Received++;
    m_numberMessagesReceived++;

    filename << filepath << "/ReceivedMessages_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.receivercallback - Tag 5" << std::endl;}
        else 
          { logFile << "Time\tLatency\tFrom\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

      logFile.close();
    }

    logFile.open(filename.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.receivercallback - Tag 5 " << std::endl;}
      else 
        { 
          logFile << time << '\t' << latency << '\t' << from <<'\t' << position.x << "," 
      << position.y << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
          logFile << neighbourhood.at(i).ip << ",";
          Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
          logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
        }
      logFile << std::endl;
      }

      
      logFile.close();

      filename2 << filepath << "/WarningMessagesReceived_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename2.str())) {

      logFile.open(filename2.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.receivercallback - tag 5" << std::endl;}
        else 
          { logFile << "Time\tFrom\tPosition\tTagType\tBlockedState\tMaliciousNeighbor NeighborSenders" << std::endl;}

      logFile.close();
    }


    logFile.open(filename2.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.receivercallback - tag 5" << std::endl;}
      else 
        { 
          logFile << time << '\t' << from <<'\t' << position.x << "," 
      << position.y << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << '0' << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) 
          {logFile << neighbourhood.at(i).ip;}
      logFile << std::endl;
      }


    break;
    case 6:

      //-------------------------------------
      // The message received is a check of a suspicious neighbor
      //-------------------------------------

    m_tag6Received++;
    m_numberMessagesReceived++;

    filename << filepath << "/ReceivedMessages_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.receivercallback - Tag 5" << std::endl;}
        else 
          { logFile << "Time\tLatency\tFrom\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

      logFile.close();
    }

    logFile.open(filename.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.receivercallback - Tag 5 " << std::endl;}
      else 
        { 
          logFile << time << '\t' << latency << '\t' << from <<'\t' << position.x << "," 
      << position.y << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
          logFile << neighbourhood.at(i).ip << ",";
          Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
          logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
        }
      logFile << std::endl;
      }

      
      logFile.close();

      filename2 << filepath << "/WarningMessagesReceived_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename2.str())) {

      logFile.open(filename2.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.receivercallback - tag 5" << std::endl;}
        else 
          { logFile << "Time\tFrom\tPosition\tTagType\tBlockedState\tMaliciousNeighbor NeighborSenders" << std::endl;}

      logFile.close();
    }


    logFile.open(filename2.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.receivercallback - tag 5" << std::endl;}
      else 
        { 
          logFile << time << '\t' << from <<'\t' << position.x << "," 
      << position.y << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << '0' << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) 
          {logFile << neighbourhood.at(i).ip;}
      logFile << std::endl;
      }

    break;
  default:
    std::cout << "Failed to read SimpleValue in statistics.receiverCallback" << std::endl;
    break;
}

  // Save information on the main tracefile

  std::stringstream globalFilename;

  globalFilename << filepath << "/AllMessages.txt";
  

  if(!fileExists(globalFilename.str())) {

    logFile.open(globalFilename.str());

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.receiverCallback - Global" << std::endl;}
      else 
        { logFile << "Time\tSender\tReceiver\tAction\tTagType\tPosition\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

    logFile.close();
  }
  
  
    // Append information on trace file

  logFile.open(globalFilename.str(),std::ios_base::app);


  if(!logFile.is_open())
      {std::cout << "Error trying to open file in statistics.receiverCallback - Global" << std::endl;}
  else 
    { logFile << time << '\t' << latency << '\t' << from << '\t' << m_nodeIP << '\t' << (int) tag.GetSimpleValue() << '\t' 
              << position.x << "," << position.y << "," << position.z << '\t' << tag.GetNNeighbors() << '\t';
    
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
      for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
        logFile << neighbourhood.at(i).ip << ",";
        Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
        logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
      }

    logFile << std::endl;
  }
  
  logFile.close();
}

double Statistics::distanceBetweenTwoPoints(Vector a,Vector b) {

  double distance = 0, distance1,distance2,distance3;

  distance1 = pow(a.x - b.x,2);
  distance2 = pow(a.y - b.y,2);
  distance3 = pow(b.z - b.z,2);

  distance = sqrt(distance1+distance2+distance3);

  return distance;
}

void Statistics::printTotalTraces() {

  std::cout << "\nTotal Traces:\n\n"
            << "Broadcasts sent: " << m_tag0Sent << "\nBroadCasts received: " << m_tag0Received << std::endl 
            << "\nAnswers to Broadcasts sent: " << m_tag1Sent << "\nAnswers to Broadcasts received: " << m_tag1Received << std::endl
            << "\nUpdates sent: " << m_tag2Sent << "\nUpdates received: " << m_tag2Received << std::endl
            << "\nWarning Messages sent: " << m_tag3Sent << "\nWarning Messages received: " << m_tag3Received << std::endl
            << "\nClearance Messages sent: " << m_tag4Sent << "\nClearance Messages received: " << m_tag4Received << std::endl
            << "\nRequest to check a neighbor sent: " << m_tag5Sent << "\nRequest to check a neighbor received: " << m_tag5Received << std::endl
            << "\nAnswer with check a neighbor sent: " << m_tag6Sent << "\nAnswer with check a neighbor received: " << m_tag6Received << std::endl
            << "\nTotal messages sent: " << m_numberMessagesSent << "\nTotal messages Received: "
            << m_numberMessagesReceived << std::endl << std::endl;

  std::ofstream logFile;
  std::stringstream filename;

  filename << filepath << "/SimulationInfo.txt";

  // Create the trace file header

  if(!fileExists(filename.str())) {

    logFile.open(filename.str());

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics - SimInfo" << std::endl;}
      else 
        { logFile << "Simulation info at " << filepath << std::endl;}

    logFile.close();
  }

  // Append information on trace file

  logFile.open(filename.str(),std::ios_base::app);

if(!logFile.is_open())
    {std::cout << "Error trying to open file in statistics - SimInfo" << std::endl;}
  else {

    logFile << "\nTotal Traces:\n\n"
            << "Broadcasts sent: " << m_tag0Sent << "\nBroadCasts received: " << m_tag0Received << std::endl 
            << "\nAnswers to Broadcasts sent: " << m_tag1Sent << "\nAnswers to Broadcasts received: " << m_tag1Received << std::endl
            << "\nUpdates sent: " << m_tag2Sent << "\nUpdates received: " << m_tag2Received << std::endl
            << "\nWarning Messages sent: " << m_tag3Sent << "\nWarning Messages received: " << m_tag3Received << std::endl
            << "\nClearance Messages sent: " << m_tag4Sent << "\nClearance Messages received: " << m_tag4Received << std::endl
            << "\nRequest to check a neighbor sent: " << m_tag5Sent << "\nRequest to check a neighbor received: " << m_tag5Received << std::endl
            << "\nAnswer with check a neighbor sent: " << m_tag6Sent << "\nAnswer with check a neighbor received: " << m_tag6Received << std::endl
            << "\nTotal messages sent: " << m_numberMessagesSent << "\nTotal messages Received: "
            << m_numberMessagesReceived << std::endl << std::endl;
            
    logFile.close();
  }

}

void Statistics::genPositionTraces(){
    
  NodeContainer::Iterator it;
  std::stringstream filename;
  std::stringstream iphelper;
  std::ofstream logFile;
  
  
  double timestamp = Simulator::Now ().ToDouble(Time::S);

  filename << filepath << "/UAVLocations.txt";

   // Create the trace file header

      if(!fileExists(filename.str())) {

        logFile.open(filename.str());

        if(!logFile.is_open())
            {std::cout << "Error trying to open file in statistics.genPositionTraces- Node Position " << std::endl;}
          else {
          
            logFile << "Time\t";
            for(unsigned int i = 0; i < NodeList::GetNNodes(); i++)
              {logFile << "UAV" << i << '\t';}

            logFile << std::endl;
          }
        logFile.close();
      }


            // Append information on trace file
    
    logFile.open(filename.str(),std::ios_base::app);

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in genPositionTraces- Node Position" << std::endl;}
    else {

      logFile << timestamp << '\t';

      for(it = NodeList::Begin(); it < NodeList::End(); it++) {

        Vector position = (*it)->GetObject<MobilityModel> ()->GetPosition();
        logFile << position << '\t';          
      }

      logFile << std::endl;
      logFile.close();
    }
  }

void Statistics::genDistanceTraces() {

  NodeContainer::Iterator it;
  vector<Vector> simulatorPositions;
  vector<Ipv4Address> simulatorIPv4;
  vector<double> neighborhoodDistances;

  for(it = NodeList::Begin(); it < NodeList::End(); it++) {

    Vector pos = (*it)->GetObject<MobilityModel> ()->GetPosition();
    simulatorPositions.push_back(pos);
    Ptr<Ipv4> ipv4 = (*it)->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1, 0);
    simulatorIPv4.push_back(iaddr.GetLocal());
  }

  for(it = NodeList::Begin(); it < NodeList::End(); it++) {

    std::vector<Ipv4Address> ipList = (*it)->GetNeighborIpList();

 
    std::ofstream logFile;
    std::stringstream filename;
    std::stringstream filename2;
    std::stringstream iphelper;
    
    Ptr<Ipv4> ipv4 = (*it)->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1, 0);
    iphelper << iaddr.GetLocal();
    std::string ipWithoutDots(iphelper.str());

    for(unsigned int i = 0; i < ipWithoutDots.size(); i++)  {
        if(ipWithoutDots[i] == '.')
          {ipWithoutDots[i] = '_';} 
    }              
  
    filename << filepath << "/Distances_" << ipWithoutDots << ".txt";

      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.genDistanceTraces" << std::endl;}
        else {
        
          logFile << "Time\t";
          for(unsigned int i = 0; i < NodeList::GetNNodes(); i++)
            {logFile << "UAV" << i << '\t';}

          logFile << std::endl;
        }
      logFile.close();
    }


      // Append information on trace file
    
    logFile.open(filename.str(),std::ios_base::app);

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in genDistanceTraces" << std::endl;}
    else {
    
      double timestamp = Simulator::Now ().ToDouble(Time::S);
      Vector thisNodePosition = (*it)->GetObject<MobilityModel> ()->GetPosition();

      logFile << timestamp << '\t';

      for( unsigned int i = 0; i < simulatorPositions.size(); i++) {
        logFile << calculateDistance( simulatorPositions.at(i),thisNodePosition) << '\t';
      }

    }

    logFile << std::endl;
    logFile.close();
  
  filename2 << filepath << "/DistanceErrors_" << ipWithoutDots << ".txt";

      // Create the trace file header

    if(!fileExists(filename2.str())) {

      logFile.open(filename2.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.genErrorDistanceTraces" << std::endl;}
        else {
        
          logFile << "Time\tMaxError\tMinError\tAverageError\tUAVErrors";
          // for(unsigned int i = 0; i < NodeList::GetNNodes(); i++)
          //   {logFile << "UAV" << i << '\t';}

          logFile << std::endl;
        }
      logFile.close();
    }


      // Append information on trace file
    
    logFile.open(filename2.str(),std::ios_base::app);

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in genErrorDistanceTraces" << std::endl;}
    else {
    
      if(ipList.size() > 0) {
      
        double timestamp = Simulator::Now ().ToDouble(Time::S);
        logFile << timestamp << '\t';

        vector<double> distanceErrors; 
        Vector thisNodePosition = (*it)->GetObject<MobilityModel> ()->GetPosition();
        
        for( unsigned int i = 0; i < simulatorIPv4.size(); i++) {

          // bool found = false;
          for(unsigned int u = 0; u < ipList.size(); u++) {
            if(simulatorIPv4.at(i) == ipList.at(u) )
              {distanceErrors.push_back(abs(calculateDistance( simulatorPositions.at(i),thisNodePosition) - (*it)->GetNeighborDistance(ipList.at(u))));}
          }
        }

        double avgError = 0;
        double maxError = 0;
        double minError = 100000;
        double value;
        for (unsigned int i = 0; i < distanceErrors.size(); i++) { 
          
          value = distanceErrors.at(i);

          if(value > maxError)
            {maxError = value;}
          
          if(value < minError)
            {minError = value;}
          
          avgError += distanceErrors.at(i); 
        }

          avgError /= distanceErrors.size();
          logFile << maxError << '\t' << minError << '\t' << avgError << '\t' << distanceErrors.size() << '\t';

        for (unsigned int i = 0; i < distanceErrors.size(); i++) 
          { logFile << distanceErrors.at(i) << '\t'; }
      

        logFile << std::endl;
      }
      logFile.close();
    }
  }
}

double Statistics::calculateDistance(Vector a, Vector b) {

  double distance = 0, distance1,distance2,distance3;

  distance1 = pow(a.x - b.x,2);
  distance2 = pow(a.y - b.y,2);
  distance3 = pow(a.z - b.z,2);

  distance = sqrt(distance1+distance2+distance3);

  return distance;
}

void Statistics::checkNeighborhoodEvolution(const Ipv4Address &node_IP, const double &time, const Ptr<Node> & m_node) {


  std::ofstream logFile;
  std::stringstream filename;
  std::stringstream iphelper;
  iphelper << node_IP;
  std::string ipWithoutDots(iphelper.str());

  for(unsigned int i = 0; i < ipWithoutDots.size(); i++)  {
      if(ipWithoutDots[i] == '.')
        {ipWithoutDots[i] = '_';} 
  }     


  filename << filepath << "/NeighborhoodEvolution_" << ipWithoutDots << ".txt";

   // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.neighborhoodEvolution" << std::endl;}
        else {
        
          logFile << "Time\tPosition\tNeighborhoodSize\tNeighbors (Ip, Position, Distance, Quality, InfoTime, Hop, State)";
          logFile << std::endl;
        }
      logFile.close();
    }


    // Append information on trace file
  
  logFile.open(filename.str(),std::ios_base::app);

  if(!logFile.is_open())
      {std::cout << "Error trying to open file in statistics.neighborhoodEvolution" << std::endl;}
  else {

    int n = m_node->GetNNeighbors();
    std::vector<Ipv4Address> ipList = m_node->GetNeighborIpList();

    logFile << time << '\t' << m_node->GetPosition() << '\t' << n << '\t';
    

    for( int i = 0; i < n; i++) {
      printNeighbor(ipList.at(i),m_node,logFile) ;
    }

    logFile << std::endl;
    logFile.close();
  }

  if(m_lastEvolution != time)
    { genNeighborHoodAnalysis();m_lastEvolution = time; }

}

void Statistics::checkMaliciousNeighborhoodEvolution(const Ipv4Address &node_IP, const double &time, const Ptr<Node> & m_node) {

  std::ofstream logFile;
  std::stringstream filename;
  std::stringstream iphelper;
  iphelper << node_IP;
  std::string ipWithoutDots(iphelper.str());

  for(unsigned int i = 0; i < ipWithoutDots.size(); i++)  {
      if(ipWithoutDots[i] == '.')
        {ipWithoutDots[i] = '_';} 
  }     


  filename << filepath << "/MaliciousNeighborListEvolution_" << ipWithoutDots << ".txt";

   // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.maliciousneighborhoodEvolution" << std::endl;}
        else {
        
          logFile << "Time\tNumberofNeighbors\tNeighbors (Ip, TimeStamp, isBlocked, Recurrence, timeToBlock)";
          logFile << std::endl;
        }
      logFile.close();
    }


    // Append information on trace file
  
  logFile.open(filename.str(),std::ios_base::app);

  if(!logFile.is_open())
      {std::cout << "Error trying to open file in statistics.maliciousneighborhoodEvolution" << std::endl;}
  else {

    int n = m_node->GetNMaliciousNeighbors();
    std::vector<Ipv4Address> ipList = m_node->GetMaliciousNeighborIpList();

    logFile << time << '\t' << n << '\t';
    
    for( int i = 0; i < n; i++) 
      { printMaliciousNeighbor(ipList.at(i),m_node,logFile); }

    logFile << std::endl;
    logFile.close();
  }
}

void Statistics::printNeighbor(Ipv4Address ip, Ptr<Node> m_node,std::ofstream &log) {

  int att = 0;

  if( (int) m_node->GetNeighborAttitude(ip) == 255) 
    att = -1;
  else if( (int) m_node->GetNeighborAttitude(ip) == 0 )
    att = 0;
  else
    {att = 1;}

  log << ip << ", " << m_node->GetNeighborPosition(ip) << ", " << m_node->GetNeighborDistance(ip) << ", " << (int) m_node->GetNeighborQuality(ip) << ", "
  << m_node->GetNeighborInfoTime(ip) << ", " << att << ", " << (int) m_node->GetNeighborHop(ip) <<  '\t' ;
  
  m_node->printNeighborSenders(ip,log);
  log << std::endl;
}

void Statistics::printMaliciousNeighbor(Ipv4Address ip, Ptr<Node> m_node,std::ofstream &log) {

  log << ip << ", " << m_node->GetMaliciousNeighborTimestamp(ip) << ", " << m_node->GetMaliciousNeighborState(ip) << ", "
  << m_node->GetMaliciousNeighborRecurrence(ip) << ", " << m_node->GetMaliciousNeighborTimeToBlock(ip) << '\t' ;
}

vector< std::pair<Ipv4Address,Vector> > Statistics::makePositionVector() {

  Ptr<Ipv4> ipv4;
  std::pair<Ipv4Address,Vector> nodeInfo;
  NodeContainer::Iterator it;
  vector< std::pair<Ipv4Address,Vector> > positions;


  for(it = NodeList::Begin(); it < NodeList::End(); it++) {

    nodeInfo.first = (*it)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
    nodeInfo.second = (*it)->GetObject<MobilityModel> ()->GetPosition();
    positions.push_back(nodeInfo);
  }

  return positions;
}

void Statistics::genNeighborHoodAnalysis() {


  vector<std::pair <Ipv4Address,Vector> > nodePositions = makePositionVector();
  
  
  for(unsigned int i = 0; i < nodePositions.size();i++) {

    // std::cout << nodePositions.at(i).first << " " << nodePositions.at(i).second << std::endl; 

  vector<Ipv4Address> neighborhood;
  vector<Ipv4Address> possibleNeighborhood;

  std::ofstream logFile;
  std::stringstream filename;
  std::stringstream iphelper;
  iphelper << nodePositions.at(i).first;
  std::string ipWithoutDots(iphelper.str());

  for(unsigned int i = 0; i < ipWithoutDots.size(); i++)  {
      if(ipWithoutDots[i] == '.')
        {ipWithoutDots[i] = '_';} 
  }     


  filename << filepath << "/NeighborhoodAnalysis_" << ipWithoutDots << ".csv";

   // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.neighborhoodAnalysis" << std::endl;}
        else {
        
          logFile << "Time;NeighborHoodSize;Neighbors;PossibleNeighborHoodSize;Possible Neighbors;Error;NeighborsFound";
        }
      logFile.close();
    }


    // Append information on trace file
  
  logFile.open(filename.str(),std::ios_base::app);

  if(!logFile.is_open())
      {std::cout << "Error trying to open file in statistics.neighborhoodAnalysis" << std::endl;}
  else {

    int n = 0;
    std::vector<Ipv4Address> ipList = NodeList::GetNode(i)->GetNeighborIpList();

    logFile << std::endl << Simulator::Now().ToDouble(Time::S) << ';';
    
    if(ipList.size() > 0) {
      
      for(unsigned int u = 0; u < ipList.size();u++) {
        
        if(NodeList::GetNode(i)->GetNeighborHop(ipList.at(u)) == 1)
          {neighborhood.push_back(ipList.at(u));n++;}
      }


    if(neighborhood.size() > 0 ) {
      logFile << n << ';' << neighborhood.at(0);

      for (unsigned int u = 1; u < neighborhood.size(); u++) {
        logFile << ' ' << neighborhood.at(u);
      }

      logFile << ';';
    } else 
      {logFile << "0;0;";}

    } else 
      {logFile << "0;0;";}
      


    Vector thisNodePosition = NodeList::GetNode(i)->GetObject<MobilityModel> ()->GetPosition();
    int possibleNeighbors = 0;


    for( unsigned int u = 0; u < nodePositions.size(); u++) {

      if(calculateDistance( thisNodePosition,nodePositions.at(u).second) < 115 && i != u)
        {possibleNeighborhood.push_back(nodePositions.at(u).first);possibleNeighbors++;}
    }

    logFile << possibleNeighbors << ';';


    if(possibleNeighborhood.size() > 0) {

    logFile << possibleNeighborhood.at(0);

    for (unsigned int u = 1; u < possibleNeighborhood.size(); u++) 
      { logFile << ' ' << possibleNeighborhood.at(u);}
    
    logFile << ';';

    } else
      {logFile << "0" << ';';}


    int correct = 0;
    for(unsigned int u = 0 ; u < possibleNeighborhood.size(); u++) {

      for (unsigned int r = 0; r < neighborhood.size(); r++) {
        if(possibleNeighborhood.at(u) == neighborhood.at(r))
          {correct++;break;}
      }

    }

    logFile << possibleNeighbors - correct << ";" << correct;

    logFile.close();
  }
  }
}

void Statistics::genMaliciousTraces() {

  NodeContainer::Iterator it;

  for(it = NodeList::Begin(); it < NodeList::End(); it++) {


    vector<Ipv4Address>maliciousIpList =  (*it)->GetMaliciousNeighborIpList();

    std::ofstream logFile;
    std::stringstream filename;
    std::stringstream iphelper;
    iphelper << (*it)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
    std::string ipWithoutDots(iphelper.str());

    for(unsigned int i = 0; i < ipWithoutDots.size(); i++)  {
        if(ipWithoutDots[i] == '.')
          {ipWithoutDots[i] = '_';} 
    }     

    filename << filepath << "/MaliciousNeighbors_" << ipWithoutDots << ".txt";

    // Create the trace file header

      if(!fileExists(filename.str())) {

        logFile.open(filename.str());

        if(!logFile.is_open())
            {std::cout << "Error trying to open file in statistics.neighborhoodEvolution" << std::endl;}
          else 
            { logFile << "Ip\tMsgSent\tMsgReceived\tisBlocked\tTimestamp\tTimeToBlock\n"; }
        logFile.close();
      }


      // Append information on trace file
    
    logFile.open(filename.str(),std::ios_base::app);

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.MaliciousNeighbors" << std::endl;}
    else {

      for (unsigned int u = 0; u < maliciousIpList.size(); u++) {
        
        logFile << maliciousIpList[u] << '\t' << (*it)->GetMaliciousNeighborMsgSent(maliciousIpList.at(u)) << '\t' 
                << (*it)->GetMaliciousNeighborMsgReceived(maliciousIpList.at(u)) << '\t' 
                << (*it)->GetMaliciousNeighborState(maliciousIpList.at(u)) << '\t' 
                << (*it)->GetMaliciousNeighborTimestamp(maliciousIpList.at(u)) << '\t'
                << (*it)->GetMaliciousNeighborTimeToBlock(maliciousIpList.at(u)) << std::endl;

      }
      logFile << std::endl;
      logFile.close();
    }    
  }

}

void Statistics::addSimulationInfo(std::string info) {

  std::ofstream logFile;
  std::stringstream filename;

  filename << filepath << "/SimulationInfo.txt";

  // Create the trace file header

  if(!fileExists(filename.str())) {

    logFile.open(filename.str());

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics - SimInfo" << std::endl;}
      else 
        { logFile << "Simulation info at " << filepath << std::endl;}

    logFile.close();
  }

  // Append information on trace file

  logFile.open(filename.str(),std::ios_base::app);

if(!logFile.is_open())
    {std::cout << "Error trying to open file in statistics - SimInfo" << std::endl;}
  else {

    logFile << info << std::endl;
    logFile.close();
  }
}

void Statistics::maliciousSenderCallback(const Ipv4Address &from , const Ipv4Address &to, MyTag &tag, const double &time, const Ptr <Node>& m_node) {

  std::ofstream logFile;
  std::stringstream iphelper;

  Vector position = tag.GetPosition() ;

  iphelper << from;
  std::string ipWithoutDots(iphelper.str());

  for(unsigned int i = 0; i < ipWithoutDots.size(); i++)  {
      if(ipWithoutDots[i] == '.')
        {ipWithoutDots[i] = '_';} 
  }

  std::stringstream filename;

  filename << filepath << "/MaliciousMessagesSent_" << ipWithoutDots << ".txt";


  // Create trace file header

  if(!fileExists(filename.str())) {

    logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.senderCallback - Malicious sent" << std::endl;}
        else 
          { logFile << "Time\tDestination\tTagType\tPosition\tNeighborhoodSize\tNeighborList (IP, Distance, Hop)" << std::endl;}

      logFile.close();
    }
  
    // Append information on trace file

  logFile.open(filename.str(),std::ios_base::app);

  if(!logFile.is_open())
      {std::cout << "Error trying to open file in statistics.senderCallback - Malicious sent" << std::endl;}
    
    else {
        
        logFile << time << '\t' << to << '\t' << (int) tag.GetSimpleValue() << '\t'
                << position.x << "," << position.y << "," << position.z << '\t' << (int) tag.GetNNeighbors() << '\t';
        
        vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
          logFile << neighbourhood.at(i).ip << ",";
          Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
          logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
        }
        }
      logFile << std::endl;
      logFile.close();

  senderCallback(from,to,tag,time,m_node);

}

void Statistics::maliciousReceiverCallback(const Ipv4Address &m_nodeIP , const Ipv4Address &from , const Ipv4Address &to, MyTag &tag, const double &time, const Ptr <Node>&m_node) {

  checkMaliciousNeighborhoodEvolution(m_nodeIP,time,m_node);

  std::ofstream logFile;  
  std::stringstream iphelper;
  Vector position = tag.GetPosition() ;
  double latency = Simulator::Now().ToDouble(Time::S) -  time;

  iphelper << m_nodeIP;
  std::string ipWithoutDots(iphelper.str());

  for(unsigned int i = 0; i < ipWithoutDots.size(); i++)  {
      if(ipWithoutDots[i] == '.')
        {ipWithoutDots[i] = '_';} 
  }

  std::stringstream filename;

  filename << filepath << "/MaliciousMessagesReceived_" << ipWithoutDots << ".txt";

    
      // Create the trace file header

    if(!fileExists(filename.str())) {

      logFile.open(filename.str());

      if(!logFile.is_open())
          {std::cout << "Error trying to open file in statistics.receivercallback - Malicious Received"<< std::endl;}
        else 
          { logFile << "Time\tLatency\tFrom\tPosition\tTagType\tNeighborhoodSize\tNeighbors (Ip, Distance, Hop)" << std::endl;}

      logFile.close();
    }

    logFile.open(filename.str(),std::ios_base::app);


      // Append information on trace file

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.receivercallback - Malicious Received" << std::endl;}
      else 
        { 
          logFile << time << '\t' << latency << '\t' << from <<'\t' << position.x << "," 
      << position.y << "," << position.z << '\t' << (int) tag.GetSimpleValue() << '\t' << tag.GetNNeighbors() << '\t';
      
      vector<MyTag::NeighInfos> neighbourhood = tag.GetNeighInfosVector();
        for(unsigned int i = 0 ; i < neighbourhood.size(); i++) {
          logFile << neighbourhood.at(i).ip << ",";
          Vector distance(neighbourhood.at(i).x,neighbourhood.at(i).y,neighbourhood.at(i).z);
          logFile << distanceBetweenTwoPoints(distance,position) << "," << (int) neighbourhood.at(i).hop << '\t';
        }
          logFile << std::endl;
        }
      logFile.close();

  receiverCallback(m_nodeIP,from,to,tag,time,latency,m_node);
}

void Statistics::suspiciousActivityCallback( const Ptr <Node>&m_node, const Ipv4Address &sender, const double &time, const double &latency, const int &status, const bool &blocked) {


  std::ofstream logFile;
  NodeContainer::Iterator it;
  std::stringstream filename;
  Ipv4Address myIp = m_node->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
  vector<Ipv4Address> possibleNeighborhood;
  vector<Ipv4Address> myNeighborIpList = m_node->GetNeighborIpList();


  for(it = NodeList::Begin(); it < NodeList::End(); it++) {
    Ptr<Ipv4> ipv4 = (*it)->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1, 0);
    if(iaddr.GetLocal() == sender) {
      possibleNeighborhood = (*it)->GetNeighborIpList();
      break;
    }
  }

filename << filepath << "/SuspiciousActivity" << ".txt";

    
    // Create the trace file header

  if(!fileExists(filename.str())) {

    logFile.open(filename.str());

    if(!logFile.is_open())
        {std::cout << "Error trying to open file in statistics.suspiciousActivityCallback" << std::endl;}
      else 
        { logFile << "Time\tSuspiciousIp\tNodeIp\tStatus\tisBlocked\tNodeNeighborhood\tSuspiciousNeighborhood\tSuspiciousNeighborhoodSize\tErrors" << std::endl;}

    logFile.close();
  }

  logFile.open(filename.str(),std::ios_base::app);


    // Append information on trace file

  if(!logFile.is_open())
      {std::cout << "Error trying to open file in statistics.suspiciousActivityCallback" << std::endl;}
    else 
      { 
        logFile << time << '\t' << latency << '\t' << sender << '\t' << myIp << '\t' << status << '\t' << (int) blocked << '\t';
        if(myNeighborIpList.size() == 0) 
          {logFile << "0";}
        else {
          for(unsigned int i = 0 ; i < myNeighborIpList.size(); i++) {
            if(i != 0) 
              {logFile << ",";}
            logFile << myNeighborIpList.at(i);
          }
        }
        
        logFile << '\t';
        
        if(possibleNeighborhood.size() == 0) 
          {logFile << "0";}
        else {
          for(unsigned int i = 0 ; i < possibleNeighborhood.size(); i++) {
            if(i != 0) 
              {logFile << ",";}
            logFile << possibleNeighborhood.at(i);
          }
        }

        int correct = 0;
        for(unsigned int u = 0 ; u < possibleNeighborhood.size(); u++) {

          if(possibleNeighborhood.at(u) == myIp)
            {correct++;continue;}

          for (unsigned int r = 0; r < myNeighborIpList.size(); r++) {
            if(possibleNeighborhood.at(u) == myNeighborIpList.at(r))
              {correct++;break;}
          }
        } 

        logFile << '\t' << possibleNeighborhood.size() << '\t' << possibleNeighborhood.size() - correct;
        logFile << std::endl;
        logFile.close();
      }


}

}