import time
import socket
import select

import sys
sys.path.append( 'messages' )

import communication_pb2
import message_ids
import constants #local file

#overwrite this in the client for callbacks
def client_callback( c_string, msg_type, msg_uid ):
	pass


class client_bsd:
	def __init__( self, callback = client_callback, name = 'Sample_Name' ): 
		self.callback_ = callback
		self.name_     = name
		self.message_prefs_ = ""
		self.timeout_ = 100 
		self.connected_ = False
		self.unique_id_ = 0
	def add_message_pref( self, pref ):
		self.message_prefs_ += chr( pref)
	def set_keepalive( self, seconds ):
		self.timeout_ = seconds
	def connect( self, address = '127.0.0.1', force_inet = False ):
		self.address_ = address
		self.send_data_ = '' #clear the two internal buffers
		self.recv_data_ = ''
		
		login_msg = communication_pb2.msg_ClientLogin()
		login_msg.name = self.name_
		login_msg.messages_accept = self.message_prefs_
		self.write( message_ids.msg_ClientLogin_ID, login_msg.SerializeToString() )

		self.connected_ = False
		try:
			if force_inet == False and address == '127.0.0.1': #unix pipes
				self.sock_fd_ = socket.socket( socket.AF_UNIX, socket.SOCK_STREAM )
				self.sock_fd_.connect( constants.UNIX_SOCKET )
				self.connected_ = True
				self.sock_fd_.setblocking( 0 )
				print "Connected to server"
				return True
			else:  #ipv4
				self.sock_fd_ = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
				self.sock_fd_.connect( (address, int(constants.PORT_NUMBER) ) )
				self.connected_ = True
				self.sock_fd_.setblocking( 0 )
				print "Connected to server"
				return True
		except:
			print "Connection failed"
			self.connected_ = False
			return False
	#returns the size of the send buffer.
	def buffer_size( self ):
		return len( self.send_data_ )
	def write( self, msg_type, data ):
		self.send_data_ += constants.create_header( msg_type, len(data), self.unique_id_ ) 
		self.send_data_ += data
	def listen( self ):
		if self.connected_ == False:
			return False
		read_set = [ self.sock_fd_ ]
		if len(self.send_data_) > 0:
			send_set = [ self.sock_fd_ ]
		else:
			send_set = []	
		inputready, outputready, _ = select.select( read_set, send_set, [], self.timeout_ )	
		for i in inputready:
			if i == self.sock_fd_:
				data = self.sock_fd_.recv( constants.BUFFER_SIZE )
				if data:
					self.recv_data_ += data
					if len(data) >= constants.MESSAGE_HEADER_SIZE:
						header = constants.get_header( self.recv_data_[:constants.MESSAGE_HEADER_SIZE] )
						self.parse_message( header )
				else:
					print "inputready error"
					connected_ = False
					return False
		for i in outputready:
			if i == self.sock_fd_:
				sent = self.sock_fd_.send( self.send_data_ )
				if sent:
					self.send_data_ = self.send_data_[sent:]
				else:
					connected_ = False
					print "outputready error"
					return False
		return True
	def disconnect( self ):
		try:
			self.sock_fd_.close()
		except:
			pass

	def parse_message( self, header ):
		msg_length    = header[1]
		msg_type      = header[0]
		msg_unique_id = header[2]
		msg_hd_sz     = constants.MESSAGE_HEADER_SIZE
		#print "length",msg_length,"  type", msg_type,"  uid", msg_unique_id	
	

		if msg_length + constants.MESSAGE_HEADER_SIZE > len(self.recv_data_):
			return False
		elif msg_type == message_ids.msg_Empty:
			pass
		elif msg_type == message_ids.msg_ServerAccept_ID:
			parser = communication_pb2.msg_ServerAccept()
			parser.ParseFromString( self.recv_data_[msg_hd_sz : msg_hd_sz+msg_length] )
			self.unique_id_ = parser.unique_id
			print "unique id(", self.unique_id_, ")"
		elif msg_type == message_ids.msg_Echo_ID:
			print "echo(", msg_unique_id, ") ", self.recv_data_[ msg_hd_sz : msg_hd_sz+msg_length ]
		else:
			 self.callback_( self.recv_data_[msg_hd_sz : msg_hd_sz+msg_length ], msg_type, msg_unique_id )
		self.recv_data_ = self.recv_data_[ msg_hd_sz + msg_length : ]

if __name__ == '__main__':
	sample = client_bsd()
	sample.add_message_pref( message_ids.msg_Echo_ID )
	if sample.connect( ):
		print "connected"
		
		while sample.listen():
			pass
	else:
		print "unsuccessful"	


