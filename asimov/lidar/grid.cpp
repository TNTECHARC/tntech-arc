#include <vector>
#include "network/client_bsd.h"  //The client connection class
#include "messages/message_ids.h"//Used for message prefs.
#include "messages/sensors.pb.h" //Used for LaserScan msg.

#include <iostream>
#include <algorithm>
#include <string>
#include <math.h>




class CartPoint
{
public:
  CartPoint( float x, float y )
  { x_ = x;
    y_ = y;
  }
  float x_;
  float y_;
};


class Grid
{
private:
  float resolution_;    //centimeters
  int size_;          //(x,y)
  std::vector<bool> grid_;
public:
  Grid( float resolution=0.1, int size=100 )
  { size_ = size;
    resolution_ = resolution;
    grid_ = std::vector<bool>( size_ * size_, true );
  }
  void set_cell( int gridX, int gridY, bool open = true )
  { if( gridX >= 0 && gridX < size_ &&
        gridY >= 0 && gridY < size_ )
    { int i = gridX + gridY * size_;
      grid_[i] = open;
    }
  }
  void set_cell( CartPoint point, bool open = true )
  { int gridX = point.x_/resolution_ + size_/2;
    int gridY = point.y_/resolution_;
    set_cell( gridX, gridY, open );
  }
  int get_i( int x, int y )
  { return x + y*size_;
  }
  void print()
  {
    for( int i = 0; i < size_; i++ )
    { for( int j = 0; j < size_; j++ )
      { if( grid_[get_i( j, i ) ] == false )
          std::cout << "#"; 
        else
          std::cout << ".";
      }
      std::cout << "\n";
    }
  }

};


//This function is called by the client class whenever data is received from the server
bool callback( std::string packet, int msg_type, int unique_id );

//Accepts command line parameters.
int main( int argc, char* argv[] )
{
  //The client constructor asks for a callback function.
  //We give it the address of the above function "callback".
  asimov::ClientBSD client( &callback );
  //The client will ask the server for this kind of message.
  client.add_message_pref( asimov::msg_LaserScan_ID );
  //This client is informally identified by this name:
  client.set_name( "grid" );

  std::string address = "127.0.0.1";
  if( argc > 1 ) //Checks if there is any command line parameters.
    address = std::string( argv[1] ); //Override the default address.

  if( client.Connect( address ) )
  { //If successful connection to server...
    while( client.is_connected() )
    { //... listen until the connection is broken.
      client.Listen();
    }
  }

  return 0;
}

//This function is called by the client class whenever data is received from the server
bool callback( std::string packet, int msg_type, int unique_id )
{ //Check for msg type.
  if( msg_type == asimov::msg_LaserScan_ID )
  { //Another error check to make sure message packet is valid as well as actual Parsing.
    asimov::msg_LaserScan lidar_scan; //Used by callback function and for filter.
    if( lidar_scan.ParseFromString( packet ) )
    { Grid grid_one;

      //std::vector<CartPoint> cart;
      for( int i = 0; i < lidar_scan.angles_size(); i++ )
      { float x = cos( lidar_scan.angles( i ) ) * lidar_scan.ranges( i );
        float y = sin( lidar_scan.angles( i ) ) * lidar_scan.ranges( i );
        //cart.push_back( CartPoint( x, y ) );
        grid_one.set_cell( CartPoint( x, y ), false );
      }
      grid_one.print();
    }
  }
  exit( 0 );
  return true;
}










