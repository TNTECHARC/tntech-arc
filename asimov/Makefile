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
client_bsd_o: network/client_bsd.cpp
	$(C13_TO_O) network/client_bsd.cpp
client_driver_bsd_o: network/client_driver_bsd.cpp
	$(C13_TO_O) network/client_driver_bsd.cpp

#Link object files into an exe
server_bsd_exe: server_driver_bsd_o server_bsd_o server_bsd_connection_o communication_pb_o
	$(LINK) ServerBSD server_bsd.o server_driver_bsd.o server_bsd_connection.o communication.pb.o $(PROTOBUF)
client_bsd_exe: client_driver_bsd_o client_bsd_o communication_pb_o
	$(LINK) ClientBSD client_bsd.o client_driver_bsd.o communication.pb.o $(PROTOBUF)

#Generates Google Protocol-Buffer messages in the subdirectory "messages"
generate_protoc: messages/Makefile
	(cd messages && make generate )


#run files
run_client_bsd:
	./ClientBSD
run_server_bsd:
	./ServerBSD
run_hokoyu_client:
	python hokoyu/client_hokoyu.py
run_obstacle_avoidance:
	python lidar/obstacle.py


