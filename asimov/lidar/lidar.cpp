#include "network/client_bsd.h"  //The client connection class
#include "messages/message_ids.h"//Used for message prefs.
#include "messages/sensors.pb.h" //Used for LaserScan msg.

#include <iostream>
#include <algorithm>
#include <string>
#include <math.h>


//#########################################################
//Author: Daniel Oliver.
//Last modified: September 14, 2013.
//Purpose: To illustrate a sample client which pulls LIDAR
//   data from the server and performs basic operations on
//   the data.
//#########################################################


//This function is called by the client class whenever data is received from the server
bool callback( std::string packet, int msg_type, int unique_id );
//Filter using openpath.
void filter_path( asimov::msg_LaserScan& lidar_scan, asimov::msg_LaserScan& result, float path_width );
//Distance of a point from a line segment. (The reference returns a scalar to be multiplied by the vector to get the closest point).
float distance_point_line( float px0, float py0, float sx, float sy, float &proj );
//A pointer to the client that will be initialized on the stack in main().
asimov::ClientBSD *client_ptr;

//The width of the robot (meters)
const float path_width = 1.0;

//Accepts command line parameters.
int main( int argc, char* argv[] )
{
  //The client constructor asks for a callback function.
  //We give it the address of the above function "callback".
  asimov::ClientBSD client( &callback );
  client_ptr = &client;
  //The client will ask the server for this kind of message.
  client.add_message_pref( asimov::msg_LaserScan_ID );
  //This client is informally identified by this name:
  client.set_name( "Obstacle avoidance" );
  //Read user input from STD::CIN
  client.ReadFromCIN( true );

  std::string address = "127.0.0.1";
  if( argc > 1 ) //Checks if there is any command line parameters.
    address = std::string( argv[1] ); //Override the default address.

  char answer;
  do
  { //Attempt to connect to the server as many times as the user wants to.
    if( client.Connect( address ) )
    { //If successful connection to server...
      while( client.is_connected() )
      { //... listen until the connection is broken.
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

//This function is called by the client class whenever data is received from the server
bool callback( std::string packet, int msg_type, int unique_id )
{ //Check for msg type.
  if( msg_type == asimov::msg_LaserScan_ID )
  { //Another error check to make sure message packet is valid as well as actual Parsing.
    asimov::msg_LaserScan lidar_scan; //Used by callback function and for filter.
    asimov::msg_LaserScan result; //The obstacle avoidance pathing result.
    if( lidar_scan.ParseFromString( packet ) )
    { filter_path( lidar_scan, result, path_width );  //The orginal lidar scan, the results to return, the path width.
      std::string potential; //Write the results to this string
      result.SerializeToString( &potential ); //The writing
      client_ptr->Write( potential, asimov::msg_LidarPotential_ID ); //Broadcast to server
    }
  }
  return true;
}
//#########################################################

//Filter using openpath.
void filter_path( asimov::msg_LaserScan& lidar_scan, asimov::msg_LaserScan& result, float path_width )
{ int path_count = lidar_scan.ranges_size();
  float half_path = path_width / 2.0;
  result.CopyFrom( lidar_scan );
  float x, y, jx, jy, proj;
  for( int i = 0; i < path_count; i++ )
  { //For each (theta, r) or laser range, in the original scan...
    x = cos( result.angles(i)) * lidar_scan.ranges( i );  //The base point
    y = sin( result.angles(i)) * lidar_scan.ranges( i );  //The base point
    for( int j = 0; j < path_count; j++ )
    { //...Check it against every other laser range in the original scan.
      jx = cos( lidar_scan.angles(j)) * lidar_scan.ranges( j ); //The testing point
      jy = sin( lidar_scan.angles(j)) * lidar_scan.ranges( j ); //The testing point
      proj = 1.0; //The ratio by which to reduce the range
      if( distance_point_line( x, y, jx, jy, proj ) <= half_path )
      { proj = std::max( 0.0f, proj ); //The range is modified by distance_point_line
        result.set_ranges( i, std::min( proj * lidar_scan.ranges( i ), result.ranges( i ) ) );
      }
    }
  }
}
//#########################################################

//Distance of a point from a line segment.
float distance_point_line( float px0, float py0, float sx, float sy, float &proj )
{ const float L2 = px0 * px0 + py0 * py0;
  if( L2 == 0.0 )
    return sqrt( sx*sx + sy*sy );
  const float t = proj = ( px0*sx + py0*sy ) / L2;
  if ( t < 0.0 )
    return sqrt( sx*sx + sy*sy );
  else if( t > 1.0 )
    return sqrt( pow(px0-sx, 2.0 ) + pow( py0-sy, 2.0 ) );
  else
    return sqrt( pow(sx - t*px0, 2.0 ) + pow( sy - t*py0, 2.0 ) ); 
}

