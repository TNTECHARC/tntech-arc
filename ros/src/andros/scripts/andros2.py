#!/usr/bin/python

from serial import Serial
import threading
from time import sleep
from sys import exit
import socket
import math

ANDROS_PORT = '/dev/ttyUSB0'
#Unified all ports via this constant.


class Andros2(object):
	def __init__(self, baudrate=9600, port = ANDROS_PORT ):
		#Give the default values.
		self.clearPacket()
		#each value pair denotes (byte, bit)
		self.cmdList_ = {'front up':(0,4),
				'front down':(0,5),
				'rear up':(0,6),
				'rear down':(0,7),
				'camera left':(1,0),
				'camera right':(1,1),
				'camera up':(1,2),
				'camera down':(1,3),
				'focus in':(1,4),
				'focus out':(1,5),
				'iris close':(1,6),
				'iris open':(1,7),
				'zoom out':(2,0),
				'zoom in':(2,1),
				'shoulder down':(4,0),
				'shoulder up':(4,1),
				'wrist up':(4,4),
				'wrist down':(4,5),
				'gripper rotate ccw':(4,6), #counter-clockwise
				'gripper rotate cw':(4,7),  #clockwise
				'gripper open':(5,0),
				'gripper close':(5,1),
				'camera extend':(10,0),
				'camera contract':(10,1),
				'wrist extend':(10,2),
				'wrist contract':(10,3),
				'graphics up':(10,4),
				'graphics down':(10,5),
				'inset v1':(13,0),
				'inset v2':(13,1),
				'main v2':(13,2),
				'main v3':(13,3),
				'main v4':(13,4)}
		#convert characters to the hexadecimal value
		self.hexDictionary_ =   {'0':0x00,
					'1':0x01,
					'2':0x02,
					'3':0x03,
					'4':0x04,
					'5':0x05,
					'6':0x06,
					'7':0x07,
					'8':0x08,
					'9':0x09,
					'a':0x0a,
					'A':0x0a,
					'b':0x0b,
					'B':0x0b,
					'c':0x0c,
					'C':0x0c,
					'd':0x0d,
					'D':0x0d,
					'e':0x0e,
					'E':0x0e,
					'f':0x0f,
					'F':0x0f}
		#wheel speeds
		self.leftWheel_  = 0
		self.rightWheel_ = 0
		#release the brakes
		self.setVehicleSafety( 'OFF' )
		# Open the serial port and start sending thread
		self.serial_port_ = Serial(port=port, baudrate=baudrate)
		self.transmitting_ = True
		self.thread_lock_ = threading.Lock()
		self.thread_ = threading.Thread(target=self.transmitPackets)
		self.thread_.setDaemon(True)
		self.thread_.start()
	###################### START COMMAND BLOCKS ######################
	#Give this command the text argument as the argument.
	def command(self, strCmd ):
		self.thread_lock_.acquire()
		try:
			self.setBit( *self.cmdList_[strCmd] )
		finally:
			self.thread_lock_.release()
	def stopCommand(self, strCmd ):
		self.thread_lock_.acquire()
		try:
			self.clrBit( *self.cmdList_[strCmd] )
		finally:
			self.thread_lock_.release()
	def setSpeed( self, left = 0, right = 0 ):
		self.thread_lock_.acquire()
		try:
			self.leftWheel_  = left  = int(left)
			self.rightWheel_ = right = int(right)
			speed = int( (left+right)/2.0 )#right + left
			pivot = int( (right-left)/2.0 )#right - left
			self.addByteToDataPacket(self.clampValue(speed) + 127, 7) #speed
			self.addByteToDataPacket(self.clampValue(pivot) + 127, 6) #pivot
			print "speed (", left, ", ", right, ")"
		finally:
			self.thread_lock_.release()
	def setSpeedPivot( self, speed = 0, pivot = 0 ):
		self.thread_lock_.acquire()
		try:
			speed = int(speed)
			pivot = int(pivot)
			self.addByteToDataPacket(self.clampValue(speed) + 127, 7) #speed
			self.addByteToDataPacket(self.clampValue(pivot) + 127, 6) #pivot
		finally:
			self.thread_lock_.release()
	###################### END COMMAND BLOCKS ######################
	def clearPacket(self):
		#The base 36-byte packet.
		self.dataPacket_ = [ord(x) for x in '\xf1\xf1\x30\x36\x30\x30\x31\x38\x32\x30\x30\x30\x39\x30\x38\x31\x38\x46\x33\x33\x30\x30\x30\x30\x30\x32\x46\x46\x30\x34\x44\x33\x30\x39\x00\x00']
		self.setDataPacketChecksum() #set the last two bits.
	def clampValue(self, cmd): 
		return max(-127, min(127,cmd))
	def addByteToDataPacket(self, newByte, byteNum):
		''' Take care of the conversion of bytes to ascii representation. '''
		nibbles = [(newByte >> 4) & 0x0F, newByte & 0x0F] #a nibble is four bits
		self.dataPacket_[byteNum*2 + 2] = ord(hex(nibbles[0])[-1:].upper())
		self.dataPacket_[byteNum*2 + 3] = ord(hex(nibbles[1])[-1:].upper())
	def getByteFromDataPacket(self, byteNum):
		''' Assemble from nibbles '''
		ascii_nibbles = []
		nibbles = self.dataPacket_[byteNum*2+2 : byteNum*2+4]
		for nibble in nibbles:
			ascii_nibbles.append(self.hexDictionary_[chr(nibble)])
		return (((ascii_nibbles[0] << 4) & 0xF0) | (ascii_nibbles[1] & 0x0F))
	def setVehicleSafety(self, cmd='ON'):
		''' Control vehicle safety switch on the robot. Inputs can be "ON" or
		    "OFF". The robot will not move nor will the articulators work if
		    the vehicle is in safety mode. '''
		currentByte = self.getByteFromDataPacket(0)
		if cmd == 'ON':
			currentByte &= 0xF9 # zero out bits 1 and 2
		elif cmd == 'OFF':
			currentByte |= 0x06 # set bits 1 and 2
		else:
		    print "Andros.setVehicleSafety: ERROR, unknown command:", cmd
		print "Commanding vehicle safety %s\n" % cmd
	def setBit(self, byteNum, bitNum):
		currentByte = self.getByteFromDataPacket(byteNum)
		currentByte |= 2**bitNum
		self.addByteToDataPacket(currentByte, byteNum)
	def clrBit(self, byteNum, bitNum):
		currentByte = self.getByteFromDataPacket(byteNum)
		currentByte &= 0xff - 2**bitNum
		self.addByteToDataPacket(currentByte, byteNum)
	def setDataPacketChecksum(self):
		''' Sum all packets and put into dataPacket positions 34 and 35 '''
		checksum = sum(self.dataPacket_[2:34])
		self.dataPacket_[34:] = [(checksum & 0x00FF),((checksum >> 8) & 0x00FF)]
	def transmitPackets(self):
		''' Set the checksum and write the packet every 40 ms '''
		while self.transmitting_:
			dataPacket_copy = []
			self.thread_lock_.acquire()
			try:
				self.setDataPacketChecksum()
				dataPacket_copy = self.dataPacket_[:]
			finally:
				self.thread_lock_.release()
			for byte in dataPacket_copy:
				self.serial_port_.write(chr(byte))
				sleep(39.5e-4)
	def outputPacket(self):
		for byte in self.dataPacket_:
		    print hex(byte),
		print "|"
	def close(self):
		self.transmitting_ = False
		self.setVehicleSafety( 'ON' )
		self.serial_port_.close()
		self.thread_.join()
############End Andros2######################################
if __name__ == '__main__':
	K_ = Andros2()
	try:
		while True:
			data = raw_input()
			if data == 'quit':
				break
			if str(data[0]).isdigit() or str(data[1]).isdigit():
				nums = data.split()
				K_.setSpeed( nums[0], nums[1] )
			elif data[0:6] == 'clear':
				K_.stopCommand( data[6:] )
			else:
				K_.command( data )
	finally:
		K_.setSpeed( 0, 0 )
		K_.close()

	


