The Intelligent Ground Vehicle Competition has three main parts
  * Waypoint navigation
  * White line recognition
  * Obstacle avoidance

In view of these elements, which are difficult to conceptually handle as a whole, I've decided to handle these as separate pieces of a whole. Therefore, there are five intertwined projects. The projects are listed here along with a summary of their goals.

## Waypoint navigation ##
**Goal** - Given a continuous input of GPS and IMU data, filter the data and output the robot's position relative to the starting point and the direction to the next waypoint.

**Possible implementation** - Ideally, a complex mathematical filter combining IMU and GPS data would be great (if there is such an equation). But initially, just using raw GPS data would be fine and evaluation of other techniques can come later. A 3d vector could be a great way to represent relative positions and directions.

**Sensors necessary** - GPS and IMU.

**Responsibilities** - Everything listed in the goals. The current short term goal is to get a prototype running with GPS asap. IMU can be integrated slowly. Also, choose the data type to represent relative positions and waypoints.

## White line recognition ##
**Goal** - Given continuous input from a webcam, an unsteady base, and continuously changing light conditions, recognize white lines on grass (similar to soccer field lines) with scattered dirt patches.

**Possible implementation** - OpenCV has most of the necessary commands and image manipulation. Custom image filters should not be the first resort.

**Sensors necessary** - Camera

**Responsibilities** - Everything listed in the goals. A good place to start is following a line of tape on the floor which is continuous unbroken color in steady light conditions and a smooth floor. Also, formulate the data to represent white lines. (Some way to represent line or curve segment equations)

## Obstacle avoidance ##
**Goal** - Given continuous input from a Lidar scanner, filter the scan to remove "ghost" or false readings. Use the scans to determine local safe paths.

**Possible implementation** - Kalman filters and Monte Carlo localization is one of the most advanced possibilities. The more likely and easier method is to just use raw data and a smooth filter.

**Sensors necessary** - A Lidar scanner (either the Hokuyo or the SICK)

**Responsibilities** - Everything listed in the goals. A good starting point is a reflex agent (no memory) to avoid immediate obstacles. Also, output the filtered scan.

## Mapping ##
**Goal** - Given all the filtered outputs from the above three modules, construct a cohesive map of the environment for the purposes of finding a path through the environment.

**Possible implementation** - A dynamic discrete probability grid where each cell represents the probability that there is an obstacle or white line there. The grid would be constantly updated as new sensor information is processed. A [heuristic search function](http://theory.stanford.edu/~amitp/GameProgramming/) combined with rudimentary motion planning would determine the route to follow to the next waypoint. Since the grid would need to be dynamic, a quadtree could be a space efficient way to handle vast amounts of area.

**Sensors necessary** - All of the above.

**Responsibilities** - Everything listed in the goals. A good starting point is using positioning information to trace the robot's progress through an empty grid before attempting to map obstacles onto the grid. Also, simple documentation will likely not suffice for this. At least a basic design document (statement of intent and outline of methods) will be required before coding begins for this.

## Hardware drivers ##
**Goal** - Write the Ubuntu serial/usb hardware drivers to interface with each sensor and actuator. The sensors have priority over the actuators.