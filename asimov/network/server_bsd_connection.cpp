#include "server_bsd_connection.h"

namespace asimov
{
/*
//Sets a bsd socket to non-blocking
int SetNonblocking( int file_descriptor )
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
}//*/


//The socket is ready to recv
int ServerBSDConnection::Recv()
{ int bytes_read = recv( fd_, buffer_, BUFFER_SIZE, 0 );
  if( bytes_read <= 0 )
  { return -1; //error
  }
  else
  {  //Read the data and store in buffer
    if( message_taken_ == true )
    { message_taken_ = false;
      stored_message_.erase( 0, MESSAGE_HEADER_SIZE + get_msg_length( stored_message_ ) );
    }
    stored_message_.append( buffer_, bytes_read );
    if( stored_message_.size() >= MESSAGE_HEADER_SIZE + get_msg_length( stored_message_ ) )
      is_message_ = true;
    else
      is_message_ = false;
    return bytes_read;
  }
}
//The socket is ready to send
int ServerBSDConnection::Send()
{ int bytes_to_write = std::min( int(write_data_.size()), BUFFER_SIZE );
  if( bytes_to_write <= 0 ) //This check is here because of a logic error in server fd_sets
  { return 0;
  }
  //Copy data to raw buffer
  write_data_.copy( buffer_, bytes_to_write, 0 );
  int bytes_written = send( fd_, buffer_, bytes_to_write, 0 );
  if( bytes_written <= 0 )
  {  //error
    return -1;
  } 
  else
  { write_data_.erase( 0, bytes_written ); //remove the data that has been sent from the buffer
    return write_data_.size();
  }
}
//Add some data to be sent
void ServerBSDConnection::Write( const std::string& data )
{ write_data_.append( data );
}
//Initialize everything
void ServerBSDConnection::Initialize( int unique_id, int file_descriptor, int address_family )
{ fd_ = file_descriptor;
  address_family_ = address_family;
  unique_id_ = unique_id;
  is_message_ = false;
  write_data_.clear();
  message_taken_ = false;
}

//Returns true if there is data waiting to be sent.
bool ServerBSDConnection::is_data_send()
{ return write_data_.size() > 0;
}
//Returns true if there is a complete message in the buffer
bool ServerBSDConnection::is_message()
{ return is_message_;
}
//Returns a complete message
std::string ServerBSDConnection::get_message()
{ int msg_len = get_msg_length( stored_message_ );
  message_taken_ = true;
  return stored_message_.substr(0, MESSAGE_HEADER_SIZE + msg_len );
}
//Get the unix file descriptor
int ServerBSDConnection::get_fd()
{ return fd_;
}
//Set the unix file descriptor
void ServerBSDConnection::set_fd( int fd )
{ fd_ = fd;
}
//Get the connection unique_id
int ServerBSDConnection::get_unique_id()
{ return unique_id_;
}
//Set the connection unique_id
void ServerBSDConnection::set_unique_id( int uid )
{ unique_id_ = uid;
}
//Returns the address family
int ServerBSDConnection::get_address_family()
{ return address_family_;
}

//Returns a pointer to the sockaddr_in
struct sockaddr_in* ServerBSDConnection::get_sockaddr_in()
{ return &sockaddr_in_;
}
//Returns a pointer to the sockaddr_un
struct sockaddr_un* ServerBSDConnection::get_sockaddr_un()
{ return &sockaddr_un_;
}

//Gets this client's message preferences
const std::string& ServerBSDConnection::get_message_prefs()
{ return message_prefs_;
}
//Sets this client's message preferences
void ServerBSDConnection::set_message_prefs( const std::string& prefs )
{ message_prefs_ = prefs;
}
//Gets this client's generic name
std::string ServerBSDConnection::get_name()
{ return name_;
}
//Sets this client's generic name
void ServerBSDConnection::set_name( std::string name )
{ name_ = name;
}

};

