#ifndef CLIENT_BSD_H
#define CLIENT_BSD_H

#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "fcntl.h"
#include <unistd.h>
#include "netdb.h"
#include <algorithm>
#include <iostream>
#include <functional>

#include "network/client_interface.h"
#include "network/message_header_bsd.h"
#include "messages/message_ids.h"

namespace asimov
{

class ClientBSD: public ClientInterface
{
public:
  ClientBSD( client_callback = [](std::string x, int msg_type, int msg_uid){ return false;} ); //Set some default values.
//IPv4 address and the port. (If 127.0.0.1 and BSD then use AF_UNIX)
  virtual bool Connect( std::string address, bool force_inet = false );
  virtual void Disconnect(); //Force disconnection
  //Loops until timeout or a complete message is waiting.
  virtual void Listen();
  //Write a parsed message to the socket
  virtual bool Write( const std::string& msg, int type );

  //Enable/disable command line.
  void ReadFromCIN( bool arg ); 

  //Command interface
  virtual void ExecuteCommand( msg_Command& message );
  virtual void ParseString( );
  virtual void ParseCommand( const std::string& cmd_line );
  
  virtual ~ClientBSD();
public:
  virtual bool is_connected(); //Returns true if valid connection
  virtual bool has_message();  //Is there a complete message in the queue?
  virtual std::string get_message(); //Returns data retrieved by listen()
  virtual void set_keepalive( int microseconds ); //The time between each keepalive message
  virtual void add_message_pref( int message_id ); //A list of message types.
  virtual void set_name( std::string name ); //A generic name that this will be called by.
  virtual int  buffer_size(); //The amount of data ready to send

private:
  bool connected_;  //Kinda obvious here.
  std::string port_;    //ipv4 port
  std::string address_; //ipv4 address
  struct timeval select_timeout_;  //Used for select function
  struct sockaddr_un sock_un_; //AF_UNIX info struct
  struct sockaddr_in sock_in_; //AF_INET info struct
  int address_family_; //AF_INET or AF_UNIX
  bool use_cin_;  //If true, read from std::cin with select()
  std::string message_prefs_;  //Valid message types.
  std::string name_;  //A generic name loosely identifying this client.
  char buffer_[ BUFFER_SIZE ]; //Internal buffer for reading/writing to.
  std::string send_data_;  //Messages waiting to be sent to the server.
  std::string recv_data_;  //A message being read in from the server.
  int unique_id_;  //An id assigned by the server and appended to each message.
  int socket_fd_;  //The socket file descriptor
  fd_set send_set_;  //For select
  fd_set recv_set_;  //For select
  client_callback callback_; //This returns true only if it processed the message successfully.
};


};
#endif

