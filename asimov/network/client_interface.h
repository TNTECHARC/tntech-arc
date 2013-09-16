#ifndef CLIENT_INTERFACE_H
#define CLIENT_INTERFACE_H

#include "network/command_interface.h"
#include "messages/message_ids.h"

#include <string>
#include <functional>

namespace asimov
{

//(message, msg type, uid of sender)
typedef std::function<bool( std::string, int, int )> client_callback;

class ClientInterface : public  CommandInterface
{
public:
//IPv4 address and the port. (If 127.0.0.1 and BSD then use AF_UNIX)
  virtual bool Connect( std::string address, bool force_inet = false ) = 0;
  virtual void Disconnect() = 0; //Force disconnection
  //Loops until timeout or a complete message is waiting.
  virtual void Listen() = 0; 
  //Write a parsed message to the socket
  virtual bool Write( const std::string& msg, int type ) = 0;

public:
  virtual int  buffer_size() = 0; //Data in send buffer.
  virtual bool is_connected() = 0; //Returns true if valid connection
  virtual bool has_message() = 0;  //Is there a complete message in the queue?
  virtual std::string get_message() = 0; //Returns same data as Listen()
  virtual void set_keepalive( int microseconds ) = 0; //The time between each keepalive message
  virtual void add_message_pref( int message_id ) = 0; //Add a message type to ask for.
  virtual void set_name( std::string name ) = 0; //A generic name that this will be called by.

};


};

#endif

