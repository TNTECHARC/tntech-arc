#ifndef MESSAGE_IDS_H
#define MESSAGE_IDS_H

namespace asimov
{
//Only [0,255] are valid values.

const int msg_Empty           = 0;
const int msg_Command_ID      = 2;
const int msg_ClientLogin_ID  = 3;
const int msg_ServerAccept_ID = 4;
const int msg_Echo_ID         = 5;
const int msg_LaserScan_ID    = 6;
const int msg_LidarPotential_ID = 7;

};

#endif

