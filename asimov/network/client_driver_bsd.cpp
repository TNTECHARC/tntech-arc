#include "network/client_bsd.h"
#include "messages/message_ids.h"

#include <string>
#include <iostream>

bool callback( std::string packet, int msg_type, int unique_id )
{

  return true; //Return true if successful parsing of this packet
}

int main( int argc, char* argv[] )
{
  asimov::ClientBSD client( &callback );
  std::string address = "127.0.0.1";
  std::string message_prefs = "";
  message_prefs.append(1, char(asimov::msg_Command_ID ) );
  message_prefs.append(1, char(asimov::msg_Echo_ID) );
  if( argc > 1 )
    address = std::string( argv[1] );
  
  client.set_name( "Sample Client" );
  client.set_message_prefs( message_prefs );
  client.ReadFromCIN( true );

  char answer;
  while( client.Connect( address, false ) == false )
  { std::cout << "Try to connect again? (y)es or (n)o?   ";
    std::cin >> answer;
    if( answer == 'n' )
      break;
  }
  while( client.is_connected() )
  { client.Listen();
  }  

  return 0;
};

