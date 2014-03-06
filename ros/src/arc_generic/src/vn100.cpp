#include "imu.h"
#include "sensor_msgs/Imu.h"
#include "ros/ros.h"


int main( int argc, char* argv[] )
{
	ros::init(argc, argv, "vn-100" );
	ros::NodeHandle node;

	asimov::IMU vn100;
	vn100.Open( "/dev/ttyUSB0" );
	ros::Publisher imu_pub = node.advertise<sensor_msgs::Imu>("imu_vn100", 1000);
	sensor_msgs::Imu result;
	while( vn100.Read( result ) && ros::ok() )
	{
		imu_pub.publish( result );
		ros::spinOnce();


	}

	return 0;
}

