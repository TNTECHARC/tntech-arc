#ifndef GPS_H
#define GPS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>

#include "messages/sensors.pb.h"

namespace asimov
{

class IMU
{
private:
  //The serial port or pseudo-socket.
  int file_desc_;
  struct termios old_tio_,
                 new_tio_;
  const static int buffer_size_ = 200;
  char buffer_[buffer_size_];

  unsigned char Checksum( char* command, int length );
public:
  //Open the serial port
  bool Open( char* port = "/dev/ttyUSB0" );
  //Close the serial port
  bool Close();
  //Get a line of data (< length)
  bool GetLine( char* line, int length );
  //Returns true if valid GPS data in the reference
  bool Read( msg_IMU& result );
  //Write a command to the serial port
  bool Write( char* line, int length );
};

};
#endif 

