import sys
#assume that this script is run from tntech-arc/asimov.
sys.path.append( "network" )  #tntech-arc/asimov/network
sys.path.append( "hokoyu" )   #tntech-arc/asimov/hokoyu
sys.path.append( "messages" ) #tntech-arc/asimov/messages

import client_bsd  #the python client
import Hoky        #the hokoyu hardware driver
import sensors_pb2 #the sensor data (LaserScan)
import message_ids #the constant IDs for messages. 


#1. Connects to the server.
#2. Asks for Laserscans from all the Lidars.
#3. Any received laserscans are given to the 'callback' function.

class obstacle_avoidance:
        #the constructor
        def __init__( self ):
                #Initialize a client_bsd with a public name to use.
                self.client_ = client_bsd.client_bsd( callback = self.callback, name = 'potential graph'  )
		#Initialize a LaserScan parsing object.
                self.laserscan_ = sensors_pb2.msg_LaserScan()
		#Ask for the LaserScan messages.
		#self.client_.add_message_pref( message_ids.msg_LidarPotential_ID )
		self.client_.add_message_pref( message_ids.msg_LaserScan_ID )
		

	def startGraph( self ):
		import matplotlib
		matplotlib.use('GTKAgg')
		import gobject
		import matplotlib.pyplot as plt
		size = 6
		self.fig = plt.figure() 
		ax = self.fig.add_axes( [0,0,1,1], polar=True, axisbg='#d5de9c' )
		self.line, = plt.polar( [[6], [6]] )
		gobject.timeout_add( 5, self.client_.listen )
		plt.show()

	def contGraph( self, theta, r ):
		self.line.set_data( theta, r )
		self.fig.canvas.draw_idle()
		return True

	#a callback that receives data from the server
	def callback( self, data, msg_type, msg_uid ):
		self.laserscan_.Clear()#clear the previous data
		self.laserscan_.ParseFromString( data )#parse the new data
		return self.contGraph( self.laserscan_.angles, self.laserscan_.ranges )

        #start the hokoyu client by connecting to the server at this  address
        def start( self,  address = "127.0.0.1" ):
                #if the client connection doesn't work, exit.
                if self.client_.connect( address ) == False:
                        return False
		#Start graphing and listening
		self.startGraph()
                #While the connection to the server is open, listen to it.
		while self.client_.listen() == True:
               		pass 
		return True

#the entry point just like "int main()"
if __name__ == '__main__':
	#Calls the __init__() on the object.
        obstacle = obstacle_avoidance()
        #if there is a command line argument, use it as the address of the server
        if len( sys.argv) < 2:
                obstacle.start()
        else:
                obstacle.start( address = argv[1] )





