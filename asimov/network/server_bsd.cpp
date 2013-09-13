#include "server_bsd.h"

namespace asimov
{

//Give the port number
void ServerBSD::Initialize( std::string port  )
{ unique_id_counter_ = 0;
  port_ = port;
  fflush( stdout );
}
//Listen on this port until closed.
void ServerBSD::Listen()
{ //Open all the ports and whatever
  OpenPorts();
  unique_id_counter_ = 1;
  int error = 0;
  //Keep the server going until a crash/Ctrl-C/Stop msg/etc.
  printf( "Listening for connections.\n"  );
  while( true )
  { //Copy the master sets to the editable sets for select.
    recv_set_ = master_recv_set_;
    send_set_ = master_send_set_;
    //Checks an array of receiving sockets, an array of writing sockets, and has an indefinite timeout.
    int sel = select( max_fd_, &recv_set_, &send_set_, NULL, 0 );
    if( sel > 0 )
    {  //Some activity has been detected.
      if( FD_ISSET( unix_fd_, &recv_set_ ) )
      { --sel;   //If there is a local connection waiting, accept it.
        Accept_UNIX();
      }
      if( FD_ISSET( ipv4_fd_, &recv_set_ ) )
      { --sel;   //There is a TCP/IP connection waiting, accept it.
        Accept_INET();
      }
      //Checks each connection's socket status.
      for( int i = 0; i < connections_.size() && sel > 0; ++i )
      {  //Checks for both recv/send
        int fd = connections_[i].get_fd();
        if( FD_ISSET( fd, &recv_set_ ) )
        { --sel;
          error = connections_[i].Recv();
          //Checks for error codes.
          if( error == -1 )
          { Disconnect( connections_[i].get_unique_id() );
            --i;
            continue;
          }
          //Checks if the connection has a complete message ready to send.
          else if( connections_[i].is_message() )
          { //Send the message out to the clients.
            Queue( connections_[i].get_message(), connections_[i].get_unique_id() );
          }
        }//End recv_set_ check
        if( FD_ISSET( fd, &send_set_ ) )
        { --sel;
          //If all messages have been sent then remove the fd from the send set.
          error = connections_[i].Send();
          if( error == -1 )
          { Disconnect( connections_[i].get_unique_id() );
            --i;
            continue;            
          }
          if( connections_[i].is_data_send() == false  )
          { FD_CLR( fd, &master_send_set_ );
          }
        }//End send_set_ check
      }//End connection loop
    }//End select
  }//End listen loop
  //Close the ports and whatever.
  ClosePorts();
}
//A full msg has been received, send it out to the appropriate clients.
void ServerBSD::Queue(  const std::string& msg, int unique_id )
{ int msg_type = get_msg_type( msg );
  if( msg_type == msg_Empty )
    return; //Nothing happened.
  ParseString( msg, unique_id );
  for( int i = 0; i < connections_.size(); i++ )
  { //Check so that the sender doesn't have a loopback. Also check that the msg type is valid for this connection.
    if( unique_id != connections_[i].get_unique_id() &&
        connections_[i].get_message_prefs().find( msg_type ) != std::string::npos )
    { connections_[i].Write( msg );
      FD_SET( connections_[i].get_fd(), &master_send_set_ );
    }
  }
}
//Give notice that a client has disconnected
void ServerBSD::Disconnect( int unique_id )
{ for( int i = 0; i < connections_.size(); i++ )
  { if( connections_[i].get_unique_id() == unique_id )
    { int fd = connections_[i].get_fd();
      printf( "Disconnection: name(%s), unique_id(%d).\n", connections_[i].get_name().c_str(), unique_id );
      close( fd );
      if( max_fd_ == fd+1 )
        max_fd_ = max_fd_ -1;
      FD_CLR( fd, &master_send_set_ );
      FD_CLR( fd, &master_recv_set_ );
      connections_.erase( connections_.begin() + i );
      return;
    }
  }
}
//Check the AF_UNIX listen port
void ServerBSD::Accept_UNIX()
{ ServerBSDConnection conn;
  socklen_t sock_size = sizeof( *conn.get_sockaddr_un() ); 
  int conn_fd = accept( unix_fd_, (struct sockaddr*)conn.get_sockaddr_un(), &sock_size );
  if( conn_fd >= 0 )
  { conn.Initialize( ++unique_id_counter_, conn_fd, AF_UNIX );
    connections_.push_back( conn );  //The client seems valid, save it.
    SetNonblocking( conn_fd );  //This client will no longer block on send/recv calls.
    FD_SET( conn_fd, &master_recv_set_ ); //The client is added to the send_set only if data is queued.
    //FD_SET( conn_fd, &master_send_set_ ); //The client needs its msg_ServerAccept
    printf( "Connection AF_UNIX: unique_id(%d), path(%s), fd(%d).\n",  conn.get_unique_id(), UNIX_SOCKET.c_str(), conn_fd );
    max_fd_ = std::max( max_fd_, conn_fd + 1 );
    //Create a message response to the client.
    msg_ServerAccept acceptance;
    acceptance.set_unique_id( conn.get_unique_id() );
    std::string message;
    acceptance.SerializeToString( &message );
    message.insert( 0, get_header( msg_ServerAccept_ID, message.size(), 0 ) );
    connections_.back().Write( message ); //Send the message to ONLY this client.
  }
}
//Check the AF_INET listen port
void ServerBSD::Accept_INET()
{ ServerBSDConnection conn; 
  sockaddr_in *new_addr_ = conn.get_sockaddr_in();
  int size_struct = sizeof( struct sockaddr_in );
  int conn_fd = accept( ipv4_fd_, (struct sockaddr*) &(*new_addr_), (socklen_t*)&size_struct );
  if( conn_fd >= 0 )
  { conn.Initialize( ++unique_id_counter_, conn_fd, AF_INET );
    connections_.push_back( conn );  //The client seems valid, save it.
    SetNonblocking( conn_fd );  //This client will no longer block on send/recv calls.
    FD_SET( conn_fd, &master_recv_set_ ); //The client is added to the send_set only if data is queued.
    //FD_SET( conn_fd, &master_send_set_ ); //The client needs its msg_ServerAccept
    printf( "Connection AF_IPV4: unique_id(%d), addr(%s:%s), fd(%d).\n", conn.get_unique_id(), 
                                     inet_ntoa( conn.get_sockaddr_in()->sin_addr ), port_.c_str(), conn_fd );
    max_fd_ = std::max( max_fd_, conn_fd + 1 );
    //Create a message response to the client.
    msg_ServerAccept acceptance;
    acceptance.set_unique_id( conn.get_unique_id() );
    std::string message;
    acceptance.SerializeToString( &message );
    message.insert( 0, get_header( msg_ServerAccept_ID, message.size(), 0 ) );
    connections_.back().Write( message ); //Send the message to ONLY this client.
  }
  else
  { printf( "AF_INET connection error.\n" );
  }
}
//Open the two server ports.
void ServerBSD::OpenPorts()
{ int port_num = htons( atoi( port_.c_str() ) );
  if( (ipv4_fd_ = socket( PF_INET, SOCK_STREAM, 0 )) < 0 )
  { printf( "AF_INET socket did not open.\n" );
    exit(0);
  }
//AF_INET ipv4 socket initialization
  memset( &ipv4_socket_, 0, sizeof( ipv4_socket_ ) );
  ipv4_socket_.sin_addr.s_addr = INADDR_ANY;
  ipv4_socket_.sin_port        = port_num;
  ipv4_socket_.sin_family      = AF_INET; 
//These next two lines will get rid of the annoying "Address already in use" after server shutdown/crash. (Copied straight from Beej's guide).
  int reuseaddr = 1;
  int err = setsockopt( ipv4_fd_, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int) );
  if( bind( ipv4_fd_, (struct sockaddr*) &ipv4_socket_, sizeof( ipv4_socket_ ) ) < 0  )
  { printf( "Error binding to AF_INET port.\n");
    exit(0);
  } 
  socklen_t ipv4_size = sizeof( ipv4_socket_ );
  getsockname( ipv4_fd_, (struct sockaddr*) &ipv4_socket_, &ipv4_size );
  printf( "AF_INET socket opened on port (%s). fd(%d).\n", port_.c_str(), ipv4_fd_ );
//AF_UNIX socket initialization.
  unix_fd_ = socket( AF_UNIX, SOCK_STREAM, 0 );
  if( unix_fd_ < 0 )
  { printf( "AF_UNIX socket did not open.\n" );
    exit(0);
  }
  memset( &unix_socket_, 0, sizeof( unix_socket_ ) );
  unix_socket_.sun_family = AF_UNIX;
  strcpy( unix_socket_.sun_path, UNIX_SOCKET.c_str() );
//Eliminated "Address already in use"
  reuseaddr = 1;
  err = setsockopt( unix_fd_, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int) );
  unlink( UNIX_SOCKET.c_str() );
  int length_sun = strlen( unix_socket_.sun_path ) + sizeof( unix_socket_.sun_family );
  if( bind( unix_fd_, (struct sockaddr* ) &unix_socket_, length_sun ) < 0 )
  { printf( "Error binding to AF_UNIX port.\n" );
    exit( 0 );
  }
  printf( "AF_UNIX socket opened on path (\"%s\"). fd(%d).\n", UNIX_SOCKET.c_str(), unix_fd_ );

//Create the listen queues.
  int queue_size = 10;
  if( listen( unix_fd_, queue_size ) >= 0 && listen( ipv4_fd_, queue_size ) >= 0 )
  { printf( "Listen queues created.\n" );
  }
  else
  { printf( "Error creating listen queue.\n" );
    exit( 0 );
  }
//Initialize the file descriptor sets
  FD_ZERO( &master_recv_set_ );
  FD_ZERO( &master_send_set_ );
  //Add the two server sockets to the recv array
  FD_SET( ipv4_fd_, &master_recv_set_ );
  FD_SET( unix_fd_, &master_recv_set_ );
  max_fd_ = std::max( unix_fd_, ipv4_fd_ ) + 1;
}
//Close the two server ports.
void ServerBSD::ClosePorts()
{ close( unix_fd_ );
  close( ipv4_fd_ );
  //Since the ports are closed. Destroy the client connections too.
  connections_.clear();
}

//Various boring commands.
void ServerBSD::ExecuteCommand( msg_Command& msg_cmd )
{
}
//Parse a string and make it do something.
void ServerBSD::ParseString( const std::string& message, int uid )
{ int unique_id = uid;
  int msg_len   = get_msg_length( message );
  int msg_type  = get_msg_type( message );
  
  std::cout << "msg type(" << msg_type << ")  msg length(" << msg_len << ")  unique id(" << unique_id << ")\n";
  //If this msg_type is of "msg_Command" then...
  if( msg_type == msg_Command_ID )
  { //This is a command message. Check to see if server is a match.
    msg_Command msg_command;
    if( msg_command.ParseFromString( message.substr( MESSAGE_HEADER_SIZE ) ) )
    { if( msg_command.target() <= 0 || msg_command.name().compare( "server" ) )
        ExecuteCommand( msg_command );
    }
  }
  if( msg_type == msg_Echo_ID )
  { std::cout << "echo(" << unique_id << ") \"" << message.substr( MESSAGE_HEADER_SIZE ) << "\"\n";
  }
  if( msg_type == msg_ClientLogin_ID  )
  { //A client is updating its server connection.
    msg_ClientLogin msg_login;
    if( msg_login.ParseFromString( message.substr( MESSAGE_HEADER_SIZE ) ) )
    { ServerBSDConnection* conn = get_conn_by_uid( unique_id ); 
      if( conn != nullptr )
      { conn->set_message_prefs( msg_login.messages_accept() );
        conn->set_name( msg_login.name() );
        //A client is only added to the master_send if it has sent a msg_ClientLogin
        FD_SET( conn->get_fd(), &master_send_set_ );
      }
    }
  }
}

//Destruct everything
ServerBSD::~ServerBSD()
{
}

void ServerBSD::set_port( const std::string& port )
{ port_ = port;
}
const std::string& ServerBSD::port()
{ return port_;
}

ServerBSDConnection* ServerBSD::get_conn_by_uid( int unique_id )
{ for( int i = 0; i < connections_.size(); i++ )
  { //Check so that the sender doesn't have a loopback. Also check that the msg type is valid for this connection.
    if( unique_id == connections_[i].get_unique_id() )
    { return &connections_[i];
    }
  }
  return nullptr;
}


};

