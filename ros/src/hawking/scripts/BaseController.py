#!/usr/bin/env python
import motorDriver
import packet_handler
import rospy
from geometry_msgs.msg import Twist


class Controller:
   
  def __init__(self):
    self.publisher = rospy.Publisher('cmd_vel', Twist )
    rospy.init_node('HawkingMotor', anonymous=True)
  def Loop(self):
    while not rospy.is_shutdown():
      self.Tick()
  def Tick(self):
    pass



if __name__ == '__main__':
  try:
    Controller()
  except rospy.ROSInterruptException: pass



