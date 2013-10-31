import sys
import threading


class Controller(object):


	def __init__(self):
		self.pipe = open('/dev/input/js0','r')
		#This lists all discrete buttons (eg. either on or off)
		#Key: Tail five bytes. Value: (Button, pressed_State, unique_ID)
		self.ButtonList_ = { #ABXY buttons
			       '01000100' : ('A',True,1),
			       '00000100' : ('A',False,2),
			       '01000101' : ('B',True,3),
			       '00000101' : ('B',False,4),
			       '01000102' : ('X',True,5),
			       '00000102' : ('X',False,6),
			       '01000103' : ('Y',True,7),
			       '00000103' : ('Y',False,8),
			       #Shoulder buttons
			       '01000104' : ('LB',True,9),
			       '00000104' : ('LB',False,10),
			       '01000105' : ('RB',True,11),
			       '00000105' : ('RB',False,12),
			       #Menu items
			       '01000107' : ('Start',True,13),
			       '00000107' : ('Start',False,14),
			       '01000106' : ('Back',True,15),
			       '00000106' : ('Back',False,16),
			       '01000108' : ('Middle',True,17),
			       '00000108' : ('Middle',False,18),
			       #The D pad
			       '0100010E' : ('D_down',True,19),
			       '0000010E' : ('D_down',False,20),
			       '0100010D' : ('D_up',True,21),
			       '0000010D' : ('D_up',False,22),
			       '0100010C' : ('D_right',True,23),
			       '0000010C' : ('D_right',False,24),
			       '0100010B' : ('D_left',True,25),
			       '0000010B' : ('D_left',False,26),
			       #Sticks down
			       '0100010A' : ('Right_stick_down',True,27),
			       '0000010A' : ('Right_stick_down',False,28),
			       '01000109' : ('Left_stick_down',True,29),
			       '00000109' : ('Left_stick_down',False,30),
		};#end button list
		#This lists all analog buttons (eg. continuous)
		#Key: Tail two bytes. Value: (Button, unique_ID, [minCenter,maxCenter], modifier)
		self.AnalogList_ = { #Joysticks
			       '0200' : ('Left_stick_horizontal',1),
			       '0201' : ('Left_stick_vertical',2),
			       '0203' : ('Right_stick_horizontal',3),
			       '0204' : ('Right_stick_vertical',4),
			       #Triggers
			       '0202' : ('Left_trigger',5),
			       '0205' : ('Right_trigger',6),
		};#end analog list

	def Process(self, hex_string, verbose = False ):
		possible = self.GetButton( hex_string )
		dead_zone = 0.3
		if possible[2]: #is a button
			print possible
		else: #is an analog
			result,num = self.DecodeAnalog( hex_string )	
			if verbose and result: print result, float(num)
	#Discrete buttons
	def GetButton(self, value ):
		try:
			return self.ButtonList_[str(value)]
		except:
			return ('NA',False,0)
	#Joysticks and triggers
	def DecodeAnalog(self, value ):
		try:
			code = ''.join( value[-4:] )	
			result = self.AnalogList_[ code ]
			val_1 = ''.join( value[2:4] ) + ''.join( value[0:2] )
			val_1 = int( val_1, 16 )
			#val_1 = float(val_1)-32768.0
			if val_1 >= 32768: val_1 = -65536 + val_1
			val_1 =  val_1 /32768.0
			#check that the movement is not in the dead zone.
			#if val_1 > result[2][1] or val_1 < result[2][0]:
			#	print self.AnalogList_[ code ], "   ", val_1
			return result,  val_1 
		except:
			return False,0
	#constantly poll for data.
	def Run(self, verbose=False):
		try:
			self.action_ = []
			while True:
				#Treat the input device as a file.
				for character in self.pipe.read(1):
					self.action_ += [character]
					#A status is 8-bytes long.
					if len(self.action_) == 8:
						byte_string = ''
						#convert each byte into two hexadecimal chars.
						for byte in self.action_:
							byte_string += ('%02X' % ord(byte) )
						byte_string = byte_string[-8:].upper() #only want last 5bytes
						self.Process( byte_string, verbose )#do something with this data.
						self.action_ = [] #ready for next 8bytes.
		except: #in case of an error
			#STOPS everything by sending out the false signal from all buttons.
			base_hex = '0000010'
			for i in range(15):
				self.Process( base_hex + hex(i)[2:].upper() )
		finally:
			return True

if __name__ == '__main__':
	K = Controller()
	K.Run( True)










