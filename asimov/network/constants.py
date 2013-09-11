import struct

#exact copy of message_header_bsd.h
PORT_NUMBER = "2991"
UNIX_SOCKET = "ASIMOV_SERVER"
MESSAGE_HEADER_SIZE = 12
BUFFER_SIZE = 1024

#gets a tuple {msg_type, msg_len, msg_uid }
def get_header( c_string ):
	return struct.unpack( '!lll', c_string )

#header creation
def create_header( msg_type, msg_len, msg_uid ):
	return struct.pack( '!lll', msg_type, msg_len, msg_uid )

