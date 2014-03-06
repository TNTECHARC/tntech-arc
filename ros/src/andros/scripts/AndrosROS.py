#!/usr/bin/env python
import rospy
from std_msgs.msg import String
from andros2 import Andros2
from andros.msg import AndrosSpeed

#Type Andros2
K_ = 0
stop = False

def callbackString(data):
	data = data.data
	print data	

	if data == 'quit':
		stop = True
	if str(data[0]).isdigit() or str(data[1]).isdigit():
		nums = data.split()
		K_.setSpeed( nums[0], nums[1] )
	elif data[0:6] == 'clear':
		K_.stopCommand( data[6:] )
	else:
		K_.command( data )

def callbackSpeed(data):
	pass
	print "(", data.left,", ", data.right, ")"
	K_.setSpeed( data.left )

def listener():
	rospy.init_node('AndrosDriver', anonymous=True)
	rospy.Subscriber("AndrosCommands", String, callbackString)
	rospy.Subscriber("AndrosSpeed", AndrosSpeed, callbackSpeed)
	rospy.spin()


if __name__ == '__main__':
        K_ = Andros2()
        try:
		listener()
        finally:
                K_.setSpeed( 0, 0 )
                K_.close()

