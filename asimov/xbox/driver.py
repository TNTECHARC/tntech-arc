import sys
sys.path.append( '../andros' )
from time import sleep

import andros2
import xbox_360_controller as xbox


#maps xbox.ButtonList to andros2.cmdList
class driver(xbox.Controller):
	def __init__(self ):
		xbox.Controller.__init__(self)
		self.Andros_ = andros2.Andros2()
		self.speed_ = [0, 0] #(pivot, speed)
		self.turbo_ = 1.5
		self.turbo_on_ = False
	def Process(self, hex_string, verbose = False ):
		possible = self.GetButton( hex_string )
		if possible[2]: #is a button
			#STEERING
			steer = 30
			if possible[0] == 'D_up' and possible[1]:
				self.speed_[1] = steer #forward
			elif possible[0] == 'D_up' and not possible[1]:
				self.speed_[1] = 0
			elif possible[0] == 'D_down' and possible[1]:
				self.speed_[1] = -steer#backward
			elif possible[0] == 'D_down' and not possible[1]:
				self.speed_[1] = 0
			elif possible[0] == 'D_right' and possible[1]:
				self.speed_[0] = -steer#left
			elif possible[0] == 'D_right' and not possible[1]:
				self.speed_[0] = 0
			elif possible[0] == 'D_left' and possible[1]:
				self.speed_[0] = steer#right
			elif possible[0] == 'D_left' and not possible[1]:
				self.speed_[0] = 0
			#APPENDEGES
			if possible[0] == 'LB' and possible[1]:
				self.Andros_.command( 'wrist contract' )
			elif possible[0] == 'LB' and not possible[1]:
				self.Andros_.stopCommand( 'wrist contract' )
			if possible[0] == 'RB' and possible[1]:
				self.Andros_.command( 'wrist extend' )
			elif possible[0] == 'RB' and not possible[1]:
				self.Andros_.stopCommand( 'wrist extend' )
			#GRIPPERS
			if possible[0] == 'A' and possible[1]:
				self.Andros_.command( 'gripper open' )
			elif possible[0] == 'A' and not possible[1]:
				self.Andros_.stopCommand( 'gripper open' )
			if possible[0] == 'B' and possible[1]:
				self.Andros_.command( 'gripper close' )
			elif possible[0] == 'B' and not possible[1]:
				self.Andros_.stopCommand( 'gripper close' )
			#GRIPPERS ROTATE
			if possible[0] == 'X' and possible[1]:
				self.Andros_.command( 'gripper rotate cw' )
			elif possible[0] == 'X' and not possible[1]:
				self.Andros_.stopCommand( 'gripper rotate cw' )
			if possible[0] == 'Y' and possible[1]:
				self.Andros_.command( 'gripper rotate ccw' )
			elif possible[0] == 'Y' and not possible[1]:
				self.Andros_.stopCommand( 'gripper rotate ccw' )
			print possible
		else: #is an analog
			analog_,speed = self.DecodeAnalog( hex_string )	
			#if abs( speed ) < 7000:
			#	speed = 0
			dead_zone = 0.3
			if analog_ == False:
				pass
			elif analog_[1] == 6:#right trigger
				self.speed_[0] = 0
				self.speed_[1] = 0
			elif analog_[1] == 5:#left trigger
				if speed < 0.0:
					self.turbo_on_ = True
				else:
					self.turbo_on_ = False
			elif analog_[1] == 2:#left vertical joystick
				if speed > dead_zone:
					self.speed_[0] = (speed-dead_zone)/(1.0-dead_zone)
				elif speed < -dead_zone:
					self.speed_[0] = (speed+dead_zone)/(1.0-dead_zone)
				else:
					self.speed_[0] = 0.0
			elif analog_[1] == 4:#right vertical joystick
				if speed > dead_zone:
					self.speed_[1] = (speed-dead_zone)/(1.0-dead_zone)
				elif speed < -dead_zone:
					self.speed_[1] = (speed+dead_zone)/(1.0-dead_zone)
				else:
					self.speed_[1] = 0.0
			scale = 95.0
			#if self.turbo_on_:
			#	self.Andros_.setSpeed( -self.speed_[0] * scale * self.turbo_, -self.speed_[1] * scale * self.turbo_on_ )
			#else:
			self.Andros_.setSpeed( -self.speed_[0] * scale, -self.speed_[1] * scale )
if __name__ == '__main__':
	while True:
		K = 0
		try:
			K = driver()
			K.Run()
		except:
			try: 
				K.Andros_.close()
			except:
				pass
			sleep(0.5)
			print "hello"



