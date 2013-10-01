#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <vector>

#include "messages/sensors.pb.h"
#include "messages/message_ids.h"
#include "network/client_bsd.h"


void open_device( char* device ); //Opens the device
void get_laser_scan( asimov::msg_LaserScan& lidar_scan );


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
  open_device( device );

  asimov::msg_LaserScan lidar_scan;

  asimov::ClientBSD client;
  client.set_name( "Sick driver" );
  client.set_keepalive( 100 );

  char answer;
  do
  { //Attempt to connect to the server as many times as the user wants to.
    if( client.Connect( address ) )
    { //If successful connection to server...
      while( client.is_connected() )
      { //... listen until the connection is broken.
        if( client.buffer_size() == 0 )
        { get_laser_scan( lidar_scan );
          lidar_scan.SerializeToString( &line_buffer );
          client.Write( line_buffer, asimov::msg_LaserScan_ID );
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

//#########################################################
void open_device( char* device )
{
  file_desc = open( device, O_RDWR | O_NOCTTY ); //Create pseudo-socket. Valid values are greater than 0.
  if( file_desc < 0) { printf( "Unable to connect to device.\n" ); exit( 0 ); }
  tcgetattr( file_desc, &old_tio ); //Save current settings of serial port.
  memset( &new_tio, 0, sizeof(new_tio) ); //Blank out new settings
  new_tio.c_cflag = B38400 | CRTSCTS | CS8 | CLOCAL | CREAD; //19200Baudrate. 8bit bytes. Local connection. May read.
  new_tio.c_iflag = IGNPAR | ICRNL; //Ignore Parity.
  new_tio.c_oflag = 0;   
  new_tio.c_lflag = ICANON; //Canonical
  tcflush( file_desc, TCIFLUSH ); //Flush the port buffer.
  tcsetattr( file_desc, TCSANOW, &new_tio );  //Give the new settings
  strncpy( buffer, "BM\n", buffer_size ); //Init message 
  int count = write( file_desc, buffer, strlen(buffer) ); //returns bytes sent.
  if( count < 3 ) { printf( "Unable to connect to device.\n" ); exit( 0 ); }
  count = read( file_desc, buffer, 8 );
  if( count == 8 ) { printf( "Connected to device.\n" ); }
}
//#########################################################









