#include "network/client_bsd.h"  //The client connection class
#include "messages/message_ids.h"//Used for message prefs.
#include "messages/sensors.pb.h" //Used for LaserScan msg.

#include <iostream>
#include <string>





//#########################################################
//Author: Daniel Oliver.
//Last modified: September 14, 2013.
//Purpose: To illustrate a sample client which pulls LIDAR
//   data from the server and performs basic operations on
//   the data.
//#########################################################


//This function is called by the client class whenever data is received from the server
bool callback( std::string packet, int msg_type, int unique_id );


//Accepts command line parameters.
int main( int argc, char* argv[] )
{
  //The client constructor asks for a callback function.
  //We give it the address of the above function "callback".
  asimov::ClientBSD client( &callback );
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


//Global Variable declarations.
asimov::msg_LaserScan lidar_scan; //Used by callback functions. Static variable on stack.


//This function is called by the client class whenever data is received from the server
bool callback( std::string packet, int msg_type, int unique_id )
{ //Check for msg type.
  if( msg_type == asimov::msg_LaserScan_ID )
  { //Another error check to make sure message packet is valid as well as actual Parsing.
    if( lidar_scan.ParseFromString( packet ) )
    { float radian_least = 3.14159 / 2.0; //Straight ahead
      float radian_most  = 3.14159 / 2.0; //Straight ahead
      for( int i = 0; i < lidar_scan.angles_size(); i++ )
      { //Record the least/greatest angle
        radian_least = std::min( radian_least, lidar_scan.angles( i ) );
        radian_most  = std::max( radian_most,  lidar_scan.angles( i ) );
      }
      //Display the least/greatest angle in this scan.
      std::cout << "Greatest angle = " << radian_most << "     Least angle = " << radian_least << "\n";
    }
  }
  return true;
}





