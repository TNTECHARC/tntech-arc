#ifndef MESSAGE_HEADER_BSD_H
#define MESSAGE_HEADER_BSD_H

namespace asimov
{

#include <string>
#include <arpa/inet.h>

//Sets a bsd socket to non-blocking
inline int SetNonblocking( int file_descriptor )
{
  int flags;

//Posix method
#if defined(O_NONBLOCK)
  if( -1 == (flags = fcntl(file_descriptor, F_GETFL, 0 ) ) )
    flags = 0;
  return fcntl(file_descriptor, F_SETFL, flags | O_NONBLOCK );
#else
  //Old school
  flags = 1;
  return ioctl(file_descriptor, FIOBIO, &flags);
#endif
}

//CONSTANTS and utility functions
const std::string PORT_NUMBER = "2991";   //TCP/IPV4 port number.
const std::string UNIX_SOCKET = "ASIMOV_SERVER"; //The AF_UNIX path.
const int MESSAGE_HEADER_SIZE = 12;        //The default message_header size.
const int BUFFER_SIZE         = 1024;  //The generic internal buffer size.

//These three function are used to extract information from a message header
inline uint32_t get_msg_type( const std::string& arg )
{
  uint32_t temp;
  memcpy( &temp, arg.c_str(), 4 );
  return ntohl( temp );
}
inline uint32_t get_msg_length( const std::string& arg ) 
{
  uint32_t temp;
  memcpy( &temp, arg.c_str()+4, 4 );
  return ntohl( temp );
}
inline uint32_t get_msg_unique_id( const std::string& arg ) 
{
  uint32_t temp;
  memcpy( &temp, arg.c_str()+8, 4 );
  return ntohl( temp );
}

//These three functions are used to set information in a message header
inline void set_msg_type( uint32_t type, char* arg )
{
  uint32_t temp = htonl( type );
  memcpy( arg, &temp, 4 );
}
inline void set_msg_length( uint32_t length, char* arg )
{
  uint32_t temp = htonl( length );
  memcpy( arg+4, &temp, 4 );
}
inline void set_msg_unique_id( uint32_t unique_id, char* arg )
{
  uint32_t temp = htonl( unique_id );
  memcpy( arg+8, &temp, 4 );
}

//Create a message header.
inline std::string get_header( int type, int length, int unique_id )
{
  char temp[ MESSAGE_HEADER_SIZE ];
  memset( temp, 0, MESSAGE_HEADER_SIZE );
  set_msg_type( type, temp );
  set_msg_length( length, temp );
  set_msg_unique_id( unique_id, temp );
  return std::string( temp, MESSAGE_HEADER_SIZE );
}

};

#endif

