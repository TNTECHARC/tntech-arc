#include "network/client_bsd.h"
#include "messages/message_ids.h"

#include <string>
#include <iostream>



//This function is called by this client whenever data is received from the server.
bool callback( std::string packet, int msg_type, int unique_id );


int main( int argc, char* argv[] )
{
  //Give it a callback function
  asimov::ClientBSD client( &callback );
  //default address
  std::string address = "127.0.0.1";
  
  client.add_message_pref( asimov::msg_Command_ID ); //Listen to generic commands
  client.add_message_pref( asimov::msg_Echo_ID    ); //Listen to echo messages

  if( argc > 1 ) //If command line argument, it should be an address.
    address = std::string( argv[1] );
  
  //This is a generic echo client for sample purposes.
  client.set_name( "Sample Client" );
  //Listen to STD::CIN
  client.ReadFromCIN( true );

  char answer;
  //Try to connect until successful or exit
  while( client.Connect( address, false ) == false )
  { std::cout << "Try to connect again? (y)es or (n)o?   ";
    std::cin >> answer;
    if( answer == 'n' )
      break;
  }
  //While connected to the server...
  while( client.is_connected() )
  { //...listen to the server and std::cin
    client.Listen();
  }  

  return 0;
};


//This function is called by this client whenever data is received from the server.
bool callback( std::string packet, int msg_type, int unique_id )
{

  return true; //Return true if successful parsing of this packet
}


