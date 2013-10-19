#include "network/client_bsd.h"  //The client connection class
#include "messages/message_ids.h"//Used for message prefs.
#include "messages/sensors.pb.h" //Used for LaserScan msg.
#include "gps/gps_functions.h"  //Used for distance formulas

#include <iostream>
#include <algorithm>
#include <string>
#include <math.h>


//#########################################################
//Author: Daniel Oliver.
//Last modified: October 9, 2013.
//Purpose: To illustrate a positioning client.
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
  client.add_message_pref( asimov::msg_GPS_ID );
  //This client is informally identified by this name:
  client.set_name( "Positioning" );

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

asimov::msg_GPS origin; //If not empty, use as origin.
int count = 0;

//This function is called by the client class whenever data is received from the server
bool callback( std::string packet, int msg_type, int unique_id )
{ //Check for msg type.
  if( msg_type == asimov::msg_GPS_ID )
  { //Another error check to make sure message packet is valid as well as actual Parsing.
    asimov::msg_GPS GPS_scan; //Used by callback function and for filter.
    if( GPS_scan.ParseFromString( packet ) )
    {
      if( count == 0 )
      { origin = GPS_scan;
      }
      else
      { asimov::msg_Vector3 position = Distance_GPS( origin, GPS_scan );
        printf( "(%f, %f, %f)\n", position.x(), position.y(), position.z() );
      }
      
      count++;
    }
  }
  return true;
}
//#########################################################




