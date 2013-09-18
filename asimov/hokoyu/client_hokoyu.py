import sys
#assume that this script is run from "tntech-arc/asimov"
sys.path.append( "network" )   #tntech-arc/asimov/network
sys.path.append( "hokoyu" )    #tntech-arc/asimov/hokoyu
sys.path.append( "messages" )  #tntech-arc/asimov/messages


import client_bsd  #the python client
import Hoky        #the hokoyu hardware driver
import sensors_pb2 #the sensor data
import message_ids #the message identifiers


#queries sensor data and broadcasts to the server
class hokoyu_client:
	#the constructor
	def __init__( self ):
		#Initialize a client_bsd with a public name to use.
		self.client_ = client_bsd.client_bsd( name = 'hokoyu client'  )
		#initialize a LaserScan parsing object
		self.laserscan_ = sensors_pb2.msg_LaserScan()

	#start the hokoyu client and start broadcasting to the server at the given address
	def start( self, address = "127.0.0.1" ):
		#if the client connection doesn't work, exit.
		if self.client_.connect( address ) == False:
			return False
		#if the Hoky driver doesn't start, exit.
		try:
			self.hokoyu_ = Hoky.Hoky()
		except:
			return False
		#feeds LaserScans to the server as fast as the server can take it.
		while self.client_.listen() == True:
			#If the client's buffer is empty, fill it again.
			if self.client_.buffer_size() == 0:
				#clear the parsing object
				self.laserscan_.Clear()
				#the list of polar coordinates
				polar_coords = self.hokoyu_.getPolarCoords()
				#for each pair of values, check validity.
				for i in range( len( polar_coords[0] ) ):
					if polar_coords[0][i] >= 0.0 and polar_coords[0][i] <= 3.14159:
						self.laserscan_.angles.append( polar_coords[0][i] )
						self.laserscan_.ranges.append(   polar_coords[1][i] )
				#write the laserscan to the send buffer
				self.client_.write( message_ids.msg_LaserScan_ID, self.laserscan_.SerializeToString() )
		#close the hardware driver
		self.hokoyu_.close()
		return True


#the entry point
if __name__ == '__main__':
	sensor = hokoyu_client()

	#if there is a command line argument, use it as the address of the server
	if len( sys.argv) < 2:
		sensor.start()
	else:
		sensor.start( address = argv[1] )



