# The basic data flow #

This is the general picture of what is going on. Notice the arrows indicating data flow.

![http://wiki.tntech-arc.googlecode.com/git-history/master/general_data_flow.png](http://wiki.tntech-arc.googlecode.com/git-history/master/general_data_flow.png)

**The generic steps:**
  1. The sensors read information about the environment.
  1. The artificial intelligence (AI) uses this information to make an informed decision (informed guess) as to what the next action should be.
  1. The actuator carries out the AI's decision which changes the environment which is just moving the robot in our case.
  1. Rinse and repeat.
This may seem simple but every autonomous robot (or AI) follows these general three steps. The only difference is how complicated each of these three steps are.

# A bit more technical data flow #
![http://wiki.tntech-arc.googlecode.com/git/systems_diagram.png](http://wiki.tntech-arc.googlecode.com/git/systems_diagram.png)

I want you to notice four things:
  1. Each module/client is a [black box](http://en.wikipedia.org/wiki/Black-box_testing) with defined inputs/outputs. This facilitates modularity. _Example: We have two LIDAR sensors, SICK and Hokuyo. The SICK is very bulky and more accurate while the Hokuyo is relatively inaccurate but very mobile so it is used more for testing. Since they share a common data format msg\_LaserScan, they can be used interchangeably for code testing purposes._
  1. Since each module/client is run in its own process, the system is naturally computationally distributed without any need for complicated and buggy threading.
  1. Since they are separate processes, they are more robust. _Example: If the GPS hardware driver crashes unexpectedly, it doesn't affect anything else. Yes, the data flow from it will stop but no other module will crash and the GPS driver can be restarted quickly with no interruption of any other process._
  1. The data formats are not immutable yet. They might change in the future if more/less data is determined to be necessary.

# Hardware drivers #
A picture is worth a hundred lines of source code. Having said that, this is a generic view of any hardware driver if you replace "GPS" with SICK, Hokuyo, or IMU. This is bereft of implementation details and is just meant to illustrate the control flow.

![http://wiki.tntech-arc.googlecode.com/git/GPS_hardware_driver.png](http://wiki.tntech-arc.googlecode.com/git/GPS_hardware_driver.png)

**//TODO: Add more details and diagrams. (9/26/13) Daniel O.**