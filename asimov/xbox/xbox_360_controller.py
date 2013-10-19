import sys
import threading


#This lists all discrete buttons (eg. either on or off)
#Key: Tail five bytes. Value: (Button, pressed_State, unique_ID)
ButtonList = { #ABXY buttons
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
def GetButton( value ):
	try:
		return ButtonList[str(value)]
	except:
		return ('NA',False,0)
#This lists all analog buttons (eg. continuous)
#Key: Tail two bytes. Value: (Button, unique_ID, [minCenter,maxCenter], modifier)
AnalogList = { #Joysticks
               '0201' : ('Left_stick_horizontal',1,  [-4000,4000], 32767),
               '0200' : ('Left_stick_vertical',2,    [-4000,4000], 32767),
               '0203' : ('Right_stick_horizontal',3, [-4000,4000], 32767),
               '0204' : ('Right_stick_vertical',4,   [-4000,4000], 32767),
               #Triggers
               '0202' : ('Left_trigger',5,  [0,0], 0),
               '0205' : ('Right_trigger',6, [0,0], 0),
};#end analog list
def DecodeAnalog( value ):
	try:
		code = ''.join( value[-4:] )	
		result = AnalogList[ code ]
		val_1 = ''.join( value[0:4] )
		val_1 = int( val_1, 16 ) - result[3]
		#check that the movement is not in the dead zone.
		if val_1 > result[2][1] or val_1 < result[2][0]:
			print AnalogList[ code ], "   ", val_1
		return 
	except:
		return False

class Controller(object):
	def __init__(self):
		self.pipe = open('/dev/input/js0','r')
	def Process(self, hex_string ):
		possible = GetButton( hex_string )
		if possible[2]:
			print possible
		else:
			DecodeAnalog( hex_string )	
	def Run(self):
		self.action_ = []
		while True:
			for character in self.pipe.read(1):
				self.action_ += [character]
				if len(self.action_) == 8:
					byte_string = ''
					for byte in self.action_:
						byte_string += ('%02X' % ord(byte) )
					byte_string = byte_string[-8:] #only want last 5bytes
					self.Process( byte_string )
					self.action_ = [] #ready for next 8bytes.
		return True

if __name__ == '__main__':
	Controller().Run()









