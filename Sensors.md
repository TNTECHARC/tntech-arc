# Sensor data #

The input from the sensors is the only source of information for the IGVC AI. As such, it is of utmost importance to define the generic format for this data. A generic data interface is necessary to allow the physical interchange of different sensors that may behave slightly differently, yet still give the same data. In brief, _generic interfaces promotes modularity in code_. Actually processing the data is a different topic.

## [LIDAR](http://en.wikipedia.org/wiki/Lidar) ##
Concisely, Lidar "measures distance by illuminating a target with a laser and analyzing the reflected light". The same concept as radar or sonar just with light.

A Lidar has a specific height, how high it is mounted on the robot frame, and a specific field-of-view (FOV) which is number of degrees° of its vision. The Lidar only sees obstacles at that specific height which can leave blind spots such as a sidewalk curb. The image shows a Lidar with a FOV (the pink area) of about 300° which is 150° of vision to either side.
> ![http://www.acroname.com/robotics/info/articles/laser/laser_terms.jpg](http://www.acroname.com/robotics/info/articles/laser/laser_terms.jpg)
**Properties to consider**
    * Angular resolution or how many degrees are there between each laser beam.
    * Minimum/maximum ranges. An obstacle, has to be within this range to be detected.
    * Field of view (FOV). The degrees° of vision.
    * A single reading is the estimated distance of a perceived obstacle at a given angle.
    * A scan is a complete set of all possible readings.
    * Number of readings in one scan = (field-of-view / angular-resolution). _Example: If the angular resolution is 1.8° and the FOV is 180° then 180/1.8 = 100 readings in each scan. (Actually, it is 101 if we remember the 0-index)_
    * As the range of two consecutive readings increases, so does the distance between them. _Example: At 1° angular resolution and two consecutive readings each with a range of 1meter. The distance between the two readings is 1.75cm._ Inside that gap, could be an undetected obstacle, it is unlikely at such a close range or resolution but it should be kept in mind.

This list of properties is not comprehensive or complete, but it gets the highlights.

![http://www.robotsinsearch.com/sites/default/files/images/SICK-TIM310-1.png](http://www.robotsinsearch.com/sites/default/files/images/SICK-TIM310-1.png)

**Data definition**

So much more interesting and actually useful, is the data structure.
Here is the very simple definition using Google's Protocol Buffers.
```
message msg_LaserScan
{
   repeated float ranges = 1;   //Meters
   repeated float angles = 2; //Radians
}
```
A few things to point out: the assignment operation is used for internal [DDL](http://en.wikipedia.org/wiki/Data_definition_language) purposes, not value assignment. The "repeated" keyword indicates a variable length array. And most importantly, the data is defined in _[polar coordinates](http://en.wikipedia.org/wiki/Polar_coordinate_system)_ where each reading is a (distance, angle) pair. Index 'i' in each array are value pairs.

Polar coordinates can be converted to 2d points (XY).
```
x[i] = ranges[i] * cos( angles[i] ) //X-coord
y[i] = ranges[i] * sin( angles[i] ) //Y-coord
```

These are our two Lidars (our Hokoyu is mounted in a protective case):
Hokuyo RG-04LX-UG01 and a SICK LMS 100.

![http://www.hizook.com/files/users/3/SICK_LMS291.jpg](http://www.hizook.com/files/users/3/SICK_LMS291.jpg)![http://www.hokuyo-aut.jp/02sensor/07scanner/img/urg_04lx_ug01_top.jpg](http://www.hokuyo-aut.jp/02sensor/07scanner/img/urg_04lx_ug01_top.jpg)

## [GPS](http://en.wikipedia.org/wiki/Global_Positioning_System) ##
This really requires no information other than the data definition. GPS is so common that it is pointless for a rehash here.

**Data definition**

Protocol Buffers definition:
```
message msg_GPS
{
   required float longitude = 1;
   required float latitude  = 2;
}
```
Again, the assignment operation is used for internal [DDL](http://en.wikipedia.org/wiki/Data_definition_language) purposes, not value assignment. The "required" keyword indicates that the data will be there, just another DDL keyword.

Honestly, not much needs to be said about this data. Our GPS is a Ag Leader GPS 1500 4.

## [Inertial-Measurement-Unit](http://en.wikipedia.org/wiki/Inertial_measurement_unit) ##
I haven't had any actual experience with this sensor so I'm just going to give a basic data definition. This may change slightly in format later. Do notice that this sensor requires knowledge of matrices.

![http://www.autosportsensors.com/wp-content/uploads/2011/01/news21-300x246.jpg](http://www.autosportsensors.com/wp-content/uploads/2011/01/news21-300x246.jpg)

**Data definition**

Protocol Buffers definition:
```
message msg_IMU
{
   //Relative to starting rotations, not the rate of rotation.
   required float roll   = 1;  //Radians
   required float pitch  = 2;  //Radians
   required float yaw    = 3;  //Radians 
   //The accelerations have to be transformed by the rotation matrix.
   required float accelX = 4;  //Meters / second^2
   required float accelY = 5;  //Meters / second^2
   required float accelZ = 6;  //Meters / second^2
}
```
The acceleration in (X,Y,Z) is in the robot's local space. But given all this information and an accurate time estimate, velocity and position estimates can be derived.

We have two IMU's and they are VectorNav VN-100s.

## Vision ##
Vision processing is a complicated subject and the topic of a different page.

_Notes: All the images are url links, tell me if a link breaks._

**//TODO: actually finish all the sensors' informationals. 9/13/2013"**