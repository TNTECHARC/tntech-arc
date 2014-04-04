import sys
import select
import Sick_Header
import time
import math

PI = 3.14159
degToRad = PI/180.0

class Sick_client:

    def __init__( self, hostname = 'localhost', port = 55001 ):
        pass
        self.sick = Sick_Header.Ranger()

        #Setting up SICK for ContinouseMonitorMode....
        if self.sick.openPort() == True:
            if self.sick.isInResponseMode:
                while self.sick.setSickContinuousMonitorMode() != True:
                    print "Retrying..."

            self.sick.startRecvThread()

            time.sleep(1.5)

        while self.Tick() is True:
            '''Pseudo Code to add...   Makes a clean option to stop the client
            if 'q' is pressed:
                self.sick.closePort()   Closes the port to the Sick.
                sys.exit(0)    '''
            pass

    def Tick(self):

	    #I am unsure how thread safe this is
            L = self.sick.scan

            Lsize = len( L )

            if( Lsize == 0 ):
	       return True

            msg = "start," + str( Lsize )   #"N"

            x = 0.0
            y = 0.0

            i = 0
            degrees = 0.0
            while i < Lsize:   #[0,180]
	       degrees = i * degToRad   # i * PI / 180.0

	       x = math.cos( degrees ) * L[i]
	       y = math.sin( degrees ) * L[i]

	       msg = msg + "," + str(x) + "," + str(y)  #"N,x0,y0,x1,y1,...x(N-2),y(N-2),xN-1,yN-1"
	       i = i + 1

	    msg = msg + ",end,"   #"N,x0,y0,x1,y1,...,x(N-2),y(N-2),x(N-1),y(N-1),end,"
            print msg

            i, o, e = select.select( [sys.stdin], [], [], 0.3 )

            if i:
	       inputChar = sys.stdin.readline().strip()[0]
	       if inputChar == 'q' or inputChar == 'Q':
		  print 'tried to quit'
		  self.sick.closeRecvThread()
		  return False


            return True

#End of Class Sick_Client.


if __name__ == '__main__':

    Sick_client()

    sys.exit(0)


