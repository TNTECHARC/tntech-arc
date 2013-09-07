#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H

#include <string>
#include <map>

#include "network/command_interface.h"

namespace asimov
{

class ServerInterface: public CommandInterface
{
public:
  //Give the port number
  virtual void Initialize( std::string port ) = 0;
  //Listen on this port until closed.
  virtual void Listen() = 0;
  //A full msg has been received, send it out to the appropriate clients. The unique_id indicates the sender.
  virtual void Queue(  const std::string& msg, int unique_id ) = 0;
  //Give notice that a client has disconnected
  virtual void Disconnect( int unique_id ) = 0;

public:
  virtual void set_port( const std::string& port ) = 0;
  virtual const std::string& port() = 0;

};

};

#endif

