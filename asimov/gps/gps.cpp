#include "gps/gps.h"
namespace asimov
{

//#########################################################
bool GPS::Open( char* port )
{
  file_desc_ = open( port, O_RDWR | O_NOCTTY ); //Create pseudo-socket. Valid values are greater than 0.
  if( file_desc_ < 0) { printf( "Unable to connect to device.\n" ); exit( 0 ); }
  tcgetattr( file_desc_, &old_tio_ ); //Save current settings of serial port.
  memset( &new_tio_, 0, sizeof(new_tio_) ); //Blank out new settings
  new_tio_.c_cflag = B4800 | CRTSCTS | CS8 | CLOCAL | CREAD; //19200Baudrate. 8bit bytes. Local connection. May read.
  new_tio_.c_iflag = IGNPAR | ICRNL; //Ignore Parity.
  new_tio_.c_oflag = 0;
  new_tio_.c_lflag = ICANON; //Canonical
  tcflush( file_desc_, TCIOFLUSH ); //Flush the port buffer.
  tcsetattr( file_desc_, TCSANOW, &new_tio_ );  //Give the new settings
  return true;
}
//#########################################################
bool GPS::Close()
{ return true;
}
//#########################################################
bool GPS::GetLine( char* line, int length )
{ tcflush( file_desc_, TCIOFLUSH );
  char current = 0;
  int count = 0;
  //Look for '$' delimited line start
  while( current != '$' )
  { read( file_desc_, &current, 1 );
    ++count;
    if( count > 30 && current == 0 )
    { printf( "No data on serial line. (attempt %d)\n", count-30 );
    }
  }
  count = 0;
  //Look for '*' delimited line end
  while( current != '*' && length > count )
  { read( file_desc_, &current, 1 );
    line[count] = current;
    ++count;
  }
  //Cut off last ',' delmitied null line by changing the
  //char after last ',' to a '\0' which is a c-string end.
  char * pch = strrchr( line, ',' );
  if( pch != NULL )
    *(pch+1) = 0;
  return true;
}
//#########################################################
bool GPS::Read( asimov::msg_GPS& result )
{ bool found = false;
  char status[] = "GPGGA";
  while( found == false)
  { memset( buffer_, 0, buffer_size_ ); 
    GetLine( buffer_, buffer_size_ ); 
    char *pch = strtok( buffer_, "," );
    if( strcmp( status, pch ) == 0 )
    { //GPGGA response
      found = true;
      pch = strtok( NULL, "," ); //time
      pch = strtok( NULL, "," ); //latitude
      float latitude = 0.0;
      int lat_deg = ((*pch)<<8)+(*(pch+1));//bytes [0,1]
      latitude = (float)lat_deg;
      lat_deg = ((*(pch+2))<<8)+(*(pch+3));//bytes [2,3]
      latitude = (float)(lat_deg)/60.0;
      pch = strtok( NULL, "," ); //latitude sign
      if( *pch == 'S' )
        latitude = -latitude; 
      pch = strtok( NULL, "," ); //longitude
      float longitude = 0.0;
      int lon_deg = ((*pch)<<8)+(*(pch+1));//bytes [0,1]
      longitude = (float)lon_deg;
      lon_deg = ((*(pch+2))<<8)+(*(pch+3));//bytes [2,3]
      longitude = (float)(lon_deg)/60.0;
      pch = strtok( NULL, "," ); //longitude sign
      if( *pch == 'W' )
        longitude = -longitude; 
      //There are a few more lines to read data from:
      //Correction type 
      //Blank
      //HDOP
      result.Clear();
      result.set_latitude( latitude );
      result.set_longitude( longitude );
      return true;
    }	
  }
  return false;
}
//#########################################################

};

