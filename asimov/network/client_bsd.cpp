#include "client_bsd.h"


namespace asimov
{



ClientBSD::ClientBSD( client_callback callback ) //Set some default values.
{ callback_ = callback;
  connected_ = false;
  port_ = PORT_NUMBER;
  //Localhost is default.
  address_ = "127.0.0.1";
  //One second is the default timeout.
  select_timeout_.tv_sec = 1;
  select_timeout_.tv_usec = 0;
  use_cin_ = false;
  memset( &sock_un_, 0, sizeof( sock_un_ ) );
  memset( &sock_in_, 0, sizeof( sock_in_ ) );
  //IPv4-TCP is default.
  address_family_ = AF_INET; 
  memset( buffer_, 0, BUFFER_SIZE );
  name_ = "Generic Client";
  unique_id_ = 0; //The server hasn't given a unique_id_ yet.
  //Initialize AF_UNIX
  sock_un_.sun_family = AF_UNIX;
  strcpy( sock_un_.sun_path, UNIX_SOCKET.c_str() );
  //Initialize AF_INET
  sock_in_.sin_family = AF_INET;
}
//IPv4 address and the port. (If 127.0.0.1 and BSD then use AF_UNIX)
bool ClientBSD::Connect( std::string address, bool force_inet )
{
//Clear out values.
  unique_id_ = 0;
  address_ = address;
  recv_data_.clear();
  send_data_.clear();
  connected_ = false;

//Login msg
  msg_ClientLogin msg_login;
  msg_login.set_name( name_ );
  msg_login.set_messages_accept( message_prefs_ );
  std::string login_message;
  msg_login.SerializeToString( &login_message );
  Write( login_message, msg_ClientLogin_ID );

  bool af_unix_conn = ( address_.compare( "127.0.0.1" ) == 0 && force_inet == false );  
  if( af_unix_conn == false )
  {
    sock_in_.sin_port = htons( atoi( port_.c_str() ) );
    sock_in_.sin_family = AF_INET; //Specifically ipv4, NOT ipv6
    inet_pton( AF_INET, address_.c_str(), &sock_in_.sin_addr ); //convert "localhost", etc. to address
    if( ( socket_fd_ = socket( PF_INET, SOCK_STREAM, 0 )) < 0 )
    { printf( "AF_INET socket did not open.\n" );
      return false;
    }
    if( connect( socket_fd_, (struct sockaddr*)&sock_in_, sizeof(sock_in_) ) < 0 )
    { printf( "AF_INET socket connection failed.\n" );
      return false;
    }
    printf( "Connected on AF_INET to (%s:%s).\n", address_.c_str(), port_.c_str() );
    printf( "Socket fd (%d).\n", socket_fd_ );
    connected_ = true;
    SetNonblocking( socket_fd_ );
    return true;
  }
  else if( af_unix_conn == true )
  {
    if( ( socket_fd_ = socket( AF_UNIX, SOCK_STREAM, 0 )) < 0 )
    { printf( "AF_UNIX socket did not open.\n" );
      return false;
    }
    if( connect( socket_fd_, (struct sockaddr*)&sock_un_, sizeof( sock_un_ ) ) < 0 )
    { printf( "AF_UNIX socket connection failed.\n" );
      return false;
    }
    printf( "Connected on AF_UNIX path (\"%s\").\n", UNIX_SOCKET.c_str() );
    printf( "Socket fd (%d).\n", socket_fd_ );
    connected_ = true;
    SetNonblocking( socket_fd_ );
    return true;
  }
  else
  { printf( "Failed all connections.\n" );
    return false;
  }
}
void ClientBSD::Disconnect() //Force disconnection
{ printf( "Disconnected from server\n" );
  close( socket_fd_ );
  connected_ = false;
}
//Loops until timeout or a message has been received (which it returns).
void ClientBSD::Listen()
{ FD_ZERO( &send_set_ ); //two bitmasks
  FD_ZERO( &recv_set_ );
  if( use_cin_ ) //if read from cin, set bit 0
    FD_SET( 0, &recv_set_ );
  if( send_data_.size() > 0 )
    FD_SET( socket_fd_, &send_set_ );  //if data to send, set appropriate bit
  FD_SET( socket_fd_, &recv_set_ );
  struct timeval timeout = select_timeout_; //copy the timeout because it is modified
  int sel = select( socket_fd_+1, &recv_set_, &send_set_, NULL, &timeout ); 
  if( sel > 0 )
  {
    if( FD_ISSET( 0, &recv_set_ ) ) //If std::CIN
    { std::string line;
      std::getline( std::cin, line );
      ParseCommand( line );
    }
    if( FD_ISSET( socket_fd_, &recv_set_ ) ) //if data received
    { int bytes_recv = recv( socket_fd_, buffer_, BUFFER_SIZE, 0 );
      if( bytes_recv <= 0 ) 
      { connected_ = false;
        Disconnect();
        return; //exit gracefully
      }
      recv_data_.append( buffer_, bytes_recv );  //Yeah! valid data!
      ParseString( );
    }//End recv_set_
    if( FD_ISSET( socket_fd_, &send_set_ ) ) //If data to send
    { int data_count = std::min( BUFFER_SIZE, int(send_data_.size()) ); //Fit as much into buffer as possible.
      send_data_.copy( buffer_, data_count );
      int bytes_sent = send( socket_fd_, buffer_, data_count, 0 );
      if( bytes_sent <= 0 )
      { connected_ = false;
        Disconnect();
        return; //exit gracefully
      }
      send_data_.erase( 0, bytes_sent ); //get rid of data sent on
    }//End send_set_
  }
  else if( sel < 0 )
  { Disconnect();  
  }
}
//Enable/Disable reading from std::cin
void ClientBSD::ReadFromCIN( bool arg )
{ use_cin_ = arg;
}
//Write a parsed message to the socket
bool ClientBSD::Write ( const std::string& msg, int type )
{ send_data_.append( get_header( type, msg.size(), unique_id_ ) );
  send_data_.append( msg );
}

//Command interface
void ClientBSD::ExecuteCommand( msg_Command& message )
{
  std::cout << "\n" << message.command();
  for( int i = 0; i < message.arguments_size(); i++ )
  { std::cout << " -" << message.arguments( i );
  }
  std::cout << "\n";
}
void ClientBSD::ParseString( )
{ 
  if( recv_data_.size() < MESSAGE_HEADER_SIZE )
  { return; //too small to be a real message.
  }
  int msg_type   = get_msg_type( recv_data_ );
  int msg_uid    = get_msg_unique_id( recv_data_ );
  int msg_length = get_msg_length( recv_data_ );
  if( msg_type == msg_Empty || (msg_length + MESSAGE_HEADER_SIZE) > recv_data_.size() )
  { return;
  }
  else if( msg_type == msg_Command_ID )
  { msg_Command msg_cmd;
    if( msg_cmd.ParseFromString( recv_data_.substr( MESSAGE_HEADER_SIZE, msg_length ) ) )
    { ExecuteCommand( msg_cmd );
    }
  }
  else if( msg_type == msg_ServerAccept_ID )
  { msg_ServerAccept msg_accept;
    if( msg_accept.ParseFromString( recv_data_.substr( MESSAGE_HEADER_SIZE, msg_length ) ) )
    { unique_id_ = msg_accept.unique_id();
      std::cout << "unique id(" << unique_id_ << ")\n";
    }
  }
  else if( msg_type == msg_Echo_ID )
  { std::cout << "echo(" << msg_uid << ") \"" << recv_data_.substr( MESSAGE_HEADER_SIZE, msg_length ) << "\"" << std::endl;
  }
  else if( callback_( recv_data_.substr( MESSAGE_HEADER_SIZE, msg_length ), msg_type, msg_uid ) )
  { //The client callback
  }
  //And clear out the used data.
  recv_data_.erase( 0, msg_length + MESSAGE_HEADER_SIZE );
}
void ClientBSD::ParseCommand( const std::string& cmd_line )
{ 
  if( cmd_line.size() < 4 )
  { return;
  }
  else if( cmd_line.substr( 0, 3 ).compare( "cmd" ) == 0 )
  {
  }
  else if( cmd_line.substr( 0, 4 ).compare( "echo" ) == 0 )
  { Write( cmd_line.substr( 5, cmd_line.size()-5 ), msg_Echo_ID );
  }
}
int ClientBSD::buffer_size()
{ return send_data_.size();
}

ClientBSD::~ClientBSD()
{
}

bool ClientBSD::is_connected() //Returns true if valid connection
{ return connected_;
}
bool ClientBSD::has_message()  //Is there a complete message in the queue?
{ return get_msg_length( recv_data_ ) <= ( MESSAGE_HEADER_SIZE + recv_data_.size() ); 
}
std::string ClientBSD::get_message()
{ return recv_data_.substr( 0, get_msg_length( recv_data_ ) + MESSAGE_HEADER_SIZE );
}
void ClientBSD::set_keepalive( int microseconds ) //The time between each keepalive message
{ int million = 1000*1000;
  select_timeout_.tv_sec  = microseconds / million;
  select_timeout_.tv_usec = microseconds % million;
}
void ClientBSD::add_message_pref( int message_id ) //A list of message types.
{ message_prefs_.append( 1, char( message_id ) );
}
void ClientBSD::set_name( std::string name ) //A generic name that this will be called by.
{ name_ = name;
}





};

