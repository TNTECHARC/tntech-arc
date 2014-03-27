#USB/SERIAL INTERFACE
# by Chris Stringer
# Written 12 FEB 2014

#unified packet sending module for sensors
import serial
from threading import Timer

class PacketHandler:

    #constructor for any initialization commands
    def __init__(self):
        print "Packet Handler Started"

    #start serial (or USB) communication with the given port
    #I included the try/except to make debugging less annoying...except it isn't working so far
    def startSerial(self, port, baud):
        exception = ""
        try:
            self.ser = serial.Serial(port, baud)
        except(exception):
            print "Port " + port + " failed to open."

        
## SINGLE COMMANDS ##########################################################

    #Self explanatory; sends a single packet
    def sendOnce(self, packet):
        self.ser.write(packet)
        print packet

    #This doesn't actually do anything yet
    def sendQuery(self, packet):

        return response

## LOOP COMMANDS ############################################################

    #begin the loop timer and recurring commands
    #this can be called with no arguments for a blank command sent at 5 Hz
    def startLoop(self, interval = .2, packetLoop = ""):

        self.interval = interval

        self.timerLoop = Timer(self.interval, self.sendLoop)
        self.timerLoop.start()
        

    #end the loop timer to stop recurring commands
    def stopLoop(self):
        self.timerLoop.cancel()

    #set the packet to be sent repeatedly
    def setLoop(self, packet):
        self.packetLoop = packet
        self.timerLoop.cancel()   #reset the timer to send the new command immediately
        self.sendLoop()

    #recurring send function
    def sendLoop(self):
        self.ser.write(self.packetLoop)
        print self.packetLoop

        self.timerLoop = Timer(self.interval, self.sendLoop)
        self.timerLoop.start()

# EOF #######################################################################
