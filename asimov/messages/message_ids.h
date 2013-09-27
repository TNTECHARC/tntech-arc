#ifndef MESSAGE_IDS_H
#define MESSAGE_IDS_H

namespace asimov
{
//Only [0,255] are valid values.

const int msg_Empty             = 0;
const int msg_Command_ID        = 2;  //{ msg_Command }
const int msg_ClientLogin_ID    = 3;  //{ msg_ClientLogin }
const int msg_ServerAccept_ID   = 4;  //{ msg_ServerAccept }
const int msg_Echo_ID           = 5;  //{ plain ascii text }
const int msg_LaserScan_ID      = 6;  //{  msg_LaserScan }
const int msg_LidarPotential_ID = 7;  //{  msg_LaserScan }
const int msg_RobotPosition_ID  = 8;  //{  msg_Position }
const int msg_RobotWaypoint_ID  = 9;  //{  msg_Position }
const int msg_GPS_ID            = 10; //{  msg_GPS }
const int msg_IMU_ID            = 11; //{  msg_IMU }


};

#endif

