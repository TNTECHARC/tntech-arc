#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <string.h>
#include <algorithm>
#include <vector>

#include "messages/sensors.pb.h"
#include "imu/imu.h"

//#########################################################
//Daniel Oliver
//Last modified: October 02, 2013
//Description: A basic IMU driver.
//#########################################################
int main( int argc, char* argv[] )
{
  asimov::IMU imu_conn;
  asimov::msg_IMU imu_data;
  std::string line_buffer;

  double posX = 0.0;
  double posY = 0.0;
  double posZ = 0.0;

  double velX = 0.0;
  double velY = 0.0;
  double velZ = 0.0;
  
  char answer;
  do
  { //Attempt to connect to the server as many times as the user wants to.
    if( imu_conn.Open() )
    { while( imu_conn.Read( imu_data ) )
      {
        //std::cout << "Yaw(" << imu_data.yaw() << ")   Pitch(" << imu_data.pitch() << ")   Roll(" << imu_data.roll() << ")\n"; 
        //std::cout << "MagX(" << imu_data.magx() << ")   MagY(" << imu_data.magy() << ")   MagZ(" <<imu_data.magz() << ")\n"; 
        //std::cout << "AccelX(" << imu_data.accelx() << ")   AccelY(" << imu_data.accely() << ")   AccelZ(" << imu_data.accelz() << ")\n"; 
        //std::cout << "GyroX(" << imu_data.gyrox() << ")   GyroY(" << imu_data.gyroy() << ")   GyroZ(" << imu_data.gyroz() << ")\n"; 
        velX += ( imu_data.accelx() / 40.0 );
        velY += (imu_data.accely() / 40.0 );
        velZ += ( imu_data.accelz() / 40.0 );

        posX += ( velX / 40.0 );
        posY += ( velY / 40.0 );
        posZ += ( velZ / 40.0 );
        std::cout << std::left << std::setw(15) << imu_data.yaw() << std::setw(15) << imu_data.pitch() << std::setw(15) << imu_data.roll() << "\n";
        //std::cout << std::left << std::setw(15) << posX << std::setw(15) << posY << std::setw(15) << posZ << "\n";
        
	imu_data.Clear();
      }
    }
    //A standard CSC-2100 prompt.
    std::cout << "Try to connect again?  (y)es or (n)o?   ";
    std::cin >> answer;
  }
  while( answer == 'y' );

  return 0;
}







