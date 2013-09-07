#include "network/server_bsd.h"



int main( int argc, char** argv )
{
  asimov::ServerBSD serv;

  serv.Initialize();

  serv.Listen();


  return 0;
}

