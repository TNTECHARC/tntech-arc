#include "network/server_interface.h"
#include "network/server_bsd_connection.h"
#include "network/message_header_bsd.h"

#include "messages/communication.pb.h"
#include "messages/message_ids.h"

#include <string>
#include <iostream>
#include <stdio.h>

namespace asimov
{

class ServerBSD : public ServerInterface
{
public:
  //Give the port number
  virtual void Initialize( std::string port = PORT_NUMBER );
  //Listen on this port until closed.
  virtual void Listen();
  //A full msg has been received, send it out to the appropriate clients. The unique_id indicates the sender.
  virtual void Queue(  const std::string& msg, int unique_id );
  //Give notice that a client has disconnected
  virtual void Disconnect( int unique_id );
  //Check the AF_UNIX listen port
  virtual void Accept_UNIX();
  //Check the AF_INET listen port
  virtual void Accept_INET();
  //Open the two server ports.
  virtual void OpenPorts();
  //Close the two server ports.
  virtual void ClosePorts();

  //Inherited from command_interface
  virtual void ExecuteCommand( msg_Command& msg_cmd );
  virtual void ParseString( const std::string& message, int uid );

  //Destruct everything
  virtual ~ServerBSD();
public:
  virtual void set_port( const std::string& port );
  virtual const std::string& port();
  virtual ServerBSDConnection* get_conn_by_uid( int unique_id );

private:
//The default port is 2991.
  std::string port_;
//A nice big array of each connected client. (Slow insertion/removal. Fast iteration)
  std::vector< ServerBSDConnection > connections_;
  int unique_id_counter_;  //The counter for uid.
  fd_set recv_set_;  //An array of file descriptors modified by select every frame.
  fd_set send_set_;  //See above line.
  fd_set master_recv_set_; //An array of file descriptors that is coped to recv_set_ every frame.
  fd_set master_send_set_; //Same for send_set_.
  struct sockaddr_in ipv4_socket_;  //Describes an ipv4 address (something like 127.0.0.1:2991)
  struct sockaddr_un unix_socket_;  //Describes a system path (something like "ASIMOV")
  int unix_fd_;  //Receive local connections on this.
  int ipv4_fd_;  //Receive network connections on this.
  int max_fd_;   //The highest fd value plus one.
};

};


