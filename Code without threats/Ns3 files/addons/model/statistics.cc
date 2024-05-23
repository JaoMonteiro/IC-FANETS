
/*
--------------------------------------------
Code developed by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
--------------------------------------------
*/

#include "statistics.h"

namespace ns3 {

Statistics::Statistics () {

  m_tag0Sent = 0;
  m_tag0Received = 0;
  m_tag1Sent = 0;
  m_tag1Received = 0;
  m_tag2Sent = 0;
  m_tag2Received = 0;
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
          { logFile << "Time\tPosition\tNeighborhoodSize\tNeighbours (Ip, Distance, Hop)" << std::endl;}

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
            << "\nTotal messages sent: " << m_numberMessagesSent << "\nTotal messages Received: "
            << m_numberMessagesReceived << std::endl << std::endl;

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
        Vector thisNodePosition = (*it)->GetObject<MobilityModel> ()->GetPosition();
        logFile << timestamp << '\t';
        vector<double> distanceErrors; 
        
        for( unsigned int i = 0; i < simulatorIPv4.size(); i++) {

          // bool found = false;
        
          for(unsigned int u = 0; u < ipList.size(); u++) {

            if(simulatorIPv4.at(i) == ipList.at(u) && (*it)->GetNeighborHop(ipList.at(u)) == 1 ) {

              // found = true;
              distanceErrors.push_back(abs(calculateDistance( simulatorPositions.at(i),thisNodePosition) - (*it)->GetNeighborDistance(ipList.at(u))));
              // logFile << distanceErrors.at(distanceErrors.size()-1) << '\t';
            }
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

void Statistics::printNeighbor(Ipv4Address ip, Ptr<Node> m_node,std::ofstream &log) {

  int att = 0;

  if( (int) m_node->GetNeighborAttitude(ip) == 255) 
    att = -1;
  else if( (int) m_node->GetNeighborAttitude(ip) == 0 )
    att = 0;
  else
    {att = 1;}

  log << ip << ", " << m_node->GetNeighborPosition(ip) << ", " << m_node->GetNeighborDistance(ip) << ", " << (int) m_node->GetNeighborQuality(ip) << ", "
  << m_node->GetNeighborInfoTime(ip) << ", " << att << ", " << (int) m_node->GetNeighborHop(ip) << '\t' ;
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

      if(calculateDistance( thisNodePosition,nodePositions.at(u).second) < 84 && i != u)
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
}

