#include "imu/imu.h"
namespace asimov
{

//#########################################################
bool IMU::Open( char* port )
{
  file_desc_ = open( port, O_RDWR | O_NOCTTY ); //Create pseudo-socket. Valid values are greater than 0.
  if( file_desc_ < 0) { printf( "Unable to connect to device.\n" ); exit( 0 ); }
  tcgetattr( file_desc_, &old_tio_ ); //Save current settings of serial port.
  memset( &new_tio_, 0, sizeof(new_tio_) ); //Blank out new settings
  new_tio_.c_cflag = B115200 | CRTSCTS | CS8 | CLOCAL | CREAD; //19200Baudrate. 8bit bytes. Local connection. May read.
  new_tio_.c_iflag = IGNPAR | ICRNL; //Ignore Parity.
  new_tio_.c_oflag = 0;
  new_tio_.c_lflag = ICANON; //Canonical
  tcflush( file_desc_, TCIOFLUSH ); //Flush the port buffer.
  tcsetattr( file_desc_, TCSANOW, &new_tio_ );  //Give the new settings
  return true;
}
//#########################################################
bool IMU::Close()
{ close( file_desc_ );
  return true;
}
//#########################################################
bool IMU::GetLine( char* line, int length )
{ char current = 0;
  int count = 0;
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
bool IMU::Read( asimov::msg_IMU& result )
{ bool found = false;
  memset( buffer_, 0, buffer_size_ ); 
  GetLine( buffer_, buffer_size_ ); 
  double Yaw, Pitch, Roll;
  double MagX, MagY, MagZ;
  double AccelX, AccelY, AccelZ;
  double GyroX, GyroY, GyroZ;
  //printf( "%s\n", buffer_ );
  sscanf( buffer_, "%*[^,],%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", 
    &Yaw, &Pitch, &Roll,
    &MagX, &MagY, &MagZ,
    &AccelX, &AccelY, &AccelZ,
    &GyroX, &GyroY, &GyroZ );

  //Return the results
  result.Clear();
  result.set_yaw(   Yaw   );
  result.set_pitch( Pitch );
  result.set_roll(  Roll  );

  result.set_magx( MagX );
  result.set_magy( MagY );
  result.set_magz( MagZ );

  result.set_accelx( AccelX );
  result.set_accely( AccelY );
  result.set_accelz( AccelZ );

  result.set_gyrox( GyroX );
  result.set_gyroy( GyroY );
  result.set_gyroz( GyroZ );
  return true;
}
//#########################################################
bool IMU::Write( char* line, int length )
{ write( file_desc_, line, length );
  return true;
}
//#########################################################
unsigned char IMU::Checksum( char* command, int length )
{ unsigned char result = 0;
  for( int i = 0; i < length; i++ )
    result ^= (unsigned char)command[i];
  return result;
}
//#########################################################

};

