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

const int buffer_size = 100;
int file_desc;
char buffer[buffer_size];
struct termios old_tio, new_tio;
std::string line_buffer;
std::string command = "GD0044072500\n";


void open_device( char* device ); //Opens the device
//Reads all lines into given buffer
void get_all_lines( std::string& buffer );
void get_laser_scan( asimov::msg_LaserScan& lidar_scan );

//#########################################################
//Daniel Oliver
//Last modified: September 10, 2013
//Description:  A straight copy of Hoky.py except with a 
//    server connection.
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
  open_device( device );

  asimov::msg_LaserScan lidar_scan;


  asimov::ClientBSD client;
  client.set_name( "Hoky driver" );
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
  new_tio.c_cflag = B19200 | CRTSCTS | CS8 | CLOCAL | CREAD; //19200Baudrate. 8bit bytes. Local connection. May read.
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
void get_all_lines( std::string& lines )
{ //Read the buffer while the pattern doesn't match:
  //Line 0: command echo.
  //Line 1: device status
  //Line 2: timestamp
  //Line 3-N: data
  lines.clear();
  while( lines.find( command ) != 0 )
  { 
    lines.clear();
    char ch = ' ', prevCh = ' ';
    while( true )
    { read( file_desc, &ch, 1 );
      lines.push_back( ch );
      if( prevCh == '\n' && ch == '\n' )
        break;
      prevCh = ch;
    }
  }
}
//#########################################################
void get_laser_scan( asimov::msg_LaserScan& lidar_scan  )
{ strncpy( buffer, command.c_str(), buffer_size ); //Init message 
  write( file_desc, buffer, strlen( buffer ) );
  get_all_lines( line_buffer );
  line_buffer.erase( 0, line_buffer.find( '\n' )+1 ); //Remove echo line
  line_buffer.erase( 0, line_buffer.find( '\n' )+1 ); //remove status line
  line_buffer.erase( 0, line_buffer.find( '\n' )+1 ); //remove timestamp line

  lidar_scan.Clear(); //Erase all data in it.
  int index;
  //Erase every newline char.
  while( (index = line_buffer.find( '\n' )) != std::string::npos )
    line_buffer.erase( index, 1 );
  //Erase every 65th character.
  for( int i = 64; i < line_buffer.size(); i = i + 64 )
    line_buffer.erase( i, 1 );
  //Actually decode the results.
  int sv0, sv1, sv2;
  int prev = 20;
  int curr = 20;
  //printf( "%s\n", line_buffer.c_str() );
  std::vector< float > ranges;
  for( int i = 0; i < line_buffer.size()-3; i = i + 3 )
  { sv0 = line_buffer[ i + 0 ] - 48;
    sv1 = line_buffer[ i + 1 ] - 48;
    sv2 = line_buffer[ i + 2 ] - 48;
    curr = (sv0<<12)+(sv1<<6)+sv2;
    if( curr <= 20 )
      curr = prev;
    ranges.push_back( float(prev)/1000.0 );
    prev = curr;
  }
  ranges.push_back( float(curr)/1000.0 ); 
  ranges.erase( ranges.begin() );
  //Add polar coordinates
  int scan_size = ranges.size();
  float PI = 3.141592;
  float resTheta = 240.0 / float(scan_size) / 180.0 * PI;
  float angle;
  for( int i = 0; i < scan_size; i++ )
  { angle = resTheta * i - PI/6.0;
    if( angle > 0.0 && angle <= PI )
    { lidar_scan.add_ranges( ranges[i] );
      lidar_scan.add_angles( resTheta * i - PI/6.0 );
    }
  }
}
//#########################################################


