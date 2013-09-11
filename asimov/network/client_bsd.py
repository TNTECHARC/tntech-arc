import time
import socket

import constants

#overwrite this in the client for callbacks
def client_callback( c_string, msg_type, msg_uid ):
	pass


class client_bsd:
	def __init__( self, callback = client_callback, name = 'Sample_Name' ): 
		self.callback_ = callback
		self.name_     = name
		self.message_prefs_ = ""
		self.timeout_ = 1000*1000
		self.unique_id_ = 0
	def add_message_pref( self, pref ):
		self.message_prefs_.append( pref )
	def set_keepalive( self, microseconds ):
		self.timeout_ = microseconds
	def connect( self, address = '127.0.0.1', force_inet = False ):
		self.address_ = address
		self.send_data_ = '' #clear the two internal buffers
		self.recv_data_ = ''
		try:
			if force_inet == False and address == '127.0.0.1': #unix pipes
				self.sock_fd_ = socket.socket( socket.AF_UNIX, socket.SOCK_STREAM )
				self.sock_fd_.connect( constants.UNIX_SOCKET )
				return True
			else:  #ipv4
				self.sock_fd_ = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
				self.sock_fd_.connect( (address, int(constants.PORT_NUMBER) ) )
				return True
		except:
			return False
	def listen( self ):
		pass
	def disconnect( self ):
		try:
			self.sock_fd_.close()
		except:
			pass


if __name__ == '__main__':
	sample = client_bsd()
	if sample.connect( force_inet = True ):
		print "connected"
	else:
		print "unsuccessful"	


