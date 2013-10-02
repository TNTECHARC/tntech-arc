#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <vector>

#include "messages/sensors.pb.h"
#include "messages/message_ids.h"
#include "network/client_bsd.h"
#include "imu/imu.h"

//#########################################################
//Daniel Oliver
//Last modified: October 02, 2013
//Description: A basic IMU driver.
//#########################################################
int main( int argc, char* argv[] )
{
  printf( "Parameters: optional(device_name), optional(server address)\n" );
  char* device = "/dev/ttyACM0";
  std::string address = "127.0.0.1";
  if( argc == 3 )
  { address = std::string( argv[2] );
    device  = argv[1];
  }
  else if( argc == 2 )
  { device = argv[1];
  }
  asimov::IMU imu_conn;
  asimov::msg_IMU imu_data;
  std::string line_buffer;
  
  asimov::ClientBSD client;
  client.set_name( "IMU driver" );
  client.set_keepalive( 100 );

  char answer;
  do
  { //Attempt to connect to the server as many times as the user wants to.
    if( client.Connect( address ) && imu_conn.Open( device ) )
    { //If successful connection to server...
      while( client.is_connected() )
      { //... listen until the connection is broken.
        if( client.buffer_size() == 0 )
        { imu_conn.Read( imu_data );
          imu_data.SerializeToString( &line_buffer );
          client.Write( line_buffer, asimov::msg_IMU_ID );
        }
        client.Listen();
      }
    }
    //A standard CSC-2100 prompt.
    std::cout << "Try to connect again?  (y)es or (n)o?   ";
    std::cin >> answer;
  }
  while( answer == 'y' );

  return 0;
}







