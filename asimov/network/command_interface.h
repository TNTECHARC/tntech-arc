#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H

#include "messages/communication.pb.h"

namespace asimov
{


class CommandInterface
{
public:
  virtual void ExecuteCommand( msg_Command& ) {}  //this is a generic catch all
  virtual void ParseString( const std::string&, int ) {}  //Used to process raw buffer from SELECT()
  virtual void ParseCommand( const std::string& ) {}  //Used to read from command line
};

};

#endif

