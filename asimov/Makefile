all: 
	echo "This is where all the build processes will be."

INCLUDES = "."
C13_TO_O = c++ -c -std=c++0x -I$(INCLUDES)

LINK = c++ -std=c++0x  -o
PROTOBUF = -lprotobuf

#Clean it up
clean:
	rm *.o

#Compiles each cpp file into o file.
server_bsd_connection_o: network/server_bsd_connection.cpp
	$(C13_TO_O) network/server_bsd_connection.cpp
server_bsd_o: network/server_bsd.cpp
	$(C13_TO_O) network/server_bsd.cpp
server_driver_bsd_o: network/server_driver_bsd.cpp
	$(C13_TO_O) network/server_driver_bsd.cpp
communication_pb_o: messages/communication.pb.cc
	$(C13_TO_O) messages/communication.pb.cc
sensors_pb_o: messages/sensors.pb.cc
	$(C13_TO_O) messages/sensors.pb.cc
client_bsd_o: network/client_bsd.cpp
	$(C13_TO_O) network/client_bsd.cpp
client_driver_bsd_o: network/client_driver_bsd.cpp
	$(C13_TO_O) network/client_driver_bsd.cpp
lidar_o: lidar/lidar.cpp
	$(C13_TO_O) lidar/lidar.cpp
hoky_driver_o: hokoyu/hoky_driver.cpp
	$(C13_TO_O) hokoyu/hoky_driver.cpp
grid_o: lidar/grid.cpp
	$(C13_TO_O) lidar/grid.cpp
gps_o: gps/gps.cpp
	$(C13_TO_O) gps/gps.cpp
gps_driver_o: gps/gps_driver.cpp
	$(C13_TO_O) gps/gps_driver.cpp
imu_o: imu/imu.cpp
	$(C13_TO_O) imu/imu.cpp
imu_driver_o: imu/imu_driver.cpp
	$(C13_TO_O) imu/imu_driver.cpp

#Link object files into an exe
imu_driver_exe: imu_o imu_driver_o sensors_pb_o
	$(LINK) IMU_Driver imu.o imu_driver.o sensors.pb.o $(PROTOBUF)
server_bsd_exe: server_driver_bsd_o server_bsd_o server_bsd_connection_o communication_pb_o
	$(LINK) ServerBSD server_bsd.o server_driver_bsd.o server_bsd_connection.o communication.pb.o $(PROTOBUF)
client_bsd_exe: client_driver_bsd_o client_bsd_o communication_pb_o
	$(LINK) ClientBSD client_bsd.o client_driver_bsd.o communication.pb.o $(PROTOBUF)
lidar_exe: lidar_o client_bsd_o communication_pb_o sensors_pb_o
	$(LINK) Lidar_driver lidar.o client_bsd.o communication.pb.o sensors.pb.o $(PROTOBUF)
hoky_driver_exe: hoky_driver_o client_bsd_o sensors_pb_o communication_pb_o
	$(LINK) Hoky_driver hoky_driver.o client_bsd.o communication.pb.o sensors.pb.o $(PROTOBUF)
gps_driver_exe: gps_o gps_driver_o sensors_pb_o communication_pb_o client_bsd_o
	$(LINK) GPS_Driver gps.o gps_driver.o sensors.pb.o communication.pb.o client_bsd.o $(PROTOBUF)

#Generates Google Protocol-Buffer messages in the subdirectory "messages"
generate_protoc: messages/Makefile
	(cd messages && make generate )


#run files
run_imu_driver:
	./IMU_Driver
run_gps_driver:
	./GPS_Driver
run_hoky_driver:  
	./Hoky_driver #hardware
run_lidar_driver:
	./Lidar_driver #processing
run_client_bsd:
	./ClientBSD  #echo client
run_server_bsd: 
	./ServerBSD  #data server
run_hokoyu_client:
	python hokoyu/client_hokoyu.py  #
run_obstacle_avoidance:
	python lidar/obstacle.py  #obstacle avoidance


