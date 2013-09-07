#ifndef SERVER_BSD_CONNECTION_H
#define SERVER_BSD_CONNECTION_H

#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "fcntl.h"
#include <unistd.h>

#include <algorithm>
#include <iostream>

#include "message_header_bsd.h"

namespace asimov
{

const socklen_t SIZE_SOCK_UN = sizeof( struct sockaddr_un );
const socklen_t SIZE_SOCK_IN = sizeof( struct sockaddr_in );

//Sets a bsd socket to non-blocking.
//int SetNonblocking( int file_descriptor );


//A common meeting point for AF_UNIX and AF_INET
class ServerBSDConnection
{
public:
//The socket is ready to recv, returns -1 if error.
  int Recv();
//The socket is ready to send, returns -1 if error.
  int Send();
//Add some data to be sent
  void Write( const std::string& data );
//Initialize everything
  void Initialize( int unique_id, int file_descriptor, int address_family );


public:
//Returns true if there is data waiting to be sent.
  bool is_data_send();
//Returns true if there is a complete message in the buffer
  bool is_message();
//Returns a complete message
  std::string get_message();
//Get the unix file descriptor
  int get_fd();
//Set the unix file descriptor
  void set_fd( int fd);
//Get the connection unique_id
  int get_unique_id();
//Set the connection unique_id
  void set_unique_id( int uid );
//Returns the address family 
  int get_address_family();
//Returns a pointer to the sockaddr_in
  struct sockaddr_in* get_sockaddr_in();
//Returns a pointer to the sockaddr_un
  struct sockaddr_un* get_sockaddr_un();
//Gets this client's message preferences
  const std::string& get_message_prefs();
//Sets this client's message preferences
  void set_message_prefs( const std::string& prefs );
//Gets this client's generic name
  std::string get_name();
//Sets this client's generic name
  void set_name( std::string name );

private:
  struct sockaddr_in sockaddr_in_;  //If AF_INET use this.
  struct sockaddr_un sockaddr_un_;  //If AF_UNIX use this.
  int fd_;                     //The file descriptor of the socket.
  int unique_id_;              //Only this client has this identification (unique)
  int address_family_;         //Either AF_UNIX(pipes) or AF_INET (IPV4)
  bool is_message_;            //Is true if there is a whole message waiting in the stored_message_ buffer.
  std::string stored_message_; //The internal buffer that the client is sending.
  std::string write_data_;     //The internal buffer that the client will recv.
  std::string message_prefs_;  //The list of messages this this client suscribes to.
  std::string name_;           //The name is generic and can be shared by multiple clients.
  char buffer_[ BUFFER_SIZE ]; //Used as a buffer for everything internal
  bool message_taken_;         //If true, then wipe the buffer on the next recv
};





};

#endif

