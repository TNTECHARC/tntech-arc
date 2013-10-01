#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <vector>

#include "messages/sensors.pb.h"
#include "messages/message_ids.h"
#include "network/client_bsd.h"
#include "gps/gps.h"

//#########################################################
//Daniel Oliver
//Last modified: September 30, 2013
//Description:  A straight copy of GPSReader.py except with 
//    a server connection.
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
  asimov::GPS gps_conn;
  asimov::msg_GPS gps_data;
  std::string line_buffer;
  
  asimov::ClientBSD client;
  client.set_name( "GPS driver" );
  client.set_keepalive( 100 );

  char answer;
  do
  { //Attempt to connect to the server as many times as the user wants to.
    if( client.Connect( address ) && gps_conn.Open( device ) )
    { //If successful connection to server...
      while( client.is_connected() )
      { //... listen until the connection is broken.
        if( client.buffer_size() == 0 )
        { gps_conn.Read( gps_data );
          gps_data.SerializeToString( &line_buffer );
          client.Write( line_buffer, asimov::msg_GPS_ID );
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







