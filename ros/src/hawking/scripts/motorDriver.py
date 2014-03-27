
#MOTOR DRIVER
# by Chris Stringer
# Written 30 JAN 2014

#begin with "!" for action, "?" for query,
#"%" for maintenance, "~" or "^" read/write config, respectively

#TODO:  make help list, implement more commands, implement ROS, implement optional debugging flag (TB or ROS)

import cmd  #custom command line support for test functions
import packet_handler as ph


class MotorController:

    #All amounts need to be strings; the TestBench sends strings, so this is simple for the interface.
    #  However, this class may be called by other services, so the string conversion is implemented.


    # STARTUP COMMANDS #####################################################################################
    def __init__(self):
        self.PH = ph.PacketHandler()

    #open a serial connection through my packet module
    def startSerial(self, port):          
        self.PH.startSerial(port, 115200)

    #start the automated command refresh to keep the motor controller off of ESTOP
    def startLoop(self, interval = .2, initialCommand = "!M 0 0\r"):
        self.PH.startLoop(interval, initialCommand)

    # MOTOR CONTROLLER COMMANDS ##############################################################################

    #sends the value for motor 1 and motor 2 in a single packet (-1000 to 1000 each motor)
    def setMotorVel(self, amountLeft, amountRight):  
        code = "!M"
        packet = self.createPacket(code, str(amountLeft), str(amountRight))  #sends the value for motor 1 and motor 2 in a single packet (-1000 to 1000)
        self.sendLoop(packet)

    #stops the motors. This has no parameters.
    def ESTOP (self):      
        code = "!EX"
        packet = self.createPacket(code)
        self.sendPacket(packet)
    
    #disengages the estop. This has no parameters.
    def ERESUME (self):    
        code = "!MG"
        packet = self.createPacket(code)

    # COMMUNICATION FUNCTIONS ################################################################################

    #receives an unknown number of arguments and creates a single string to send
    #the code is a small command identifier for the motor controller
    def createPacket (self, code, *args): 
        packet = code
        for arg in args:
            packet += " " + arg + "\r"   

        return packet

    #this is a direct link to send a packet via the serial connection (with debugging)
    def sendPacket (self, packet):
        #self.PH.
        print packet

    #this is a modified command to send a particular command repeatedly (required for the motor driver speed)
    def sendLoop(self, packet):
        PH.setPacket(packet)


#//// TESTING FUNCTIONALITY /////////////////////////////////////////////////////////////////////////////////////////////////

class TestBench (cmd.Cmd):  
    
    def __init__(self):#, completekey='tab', stdin=None, stdout=None):
        cmd.Cmd.__init__(self)
        self.MC = MotorController()


    # CLI COMMANDS #####################################################################################################

    #The Cmd interface will recognize commands with the do_[command] convention
    # Thus, the user types 'setspeed' and the program will run 'do_setspeed'

    def do_setspeed (self, inputString):         
        if args != "":
            args = inputString.split()  

            #if only one value is given, interpret it as a full forward/back
            if len(args) == 1:                   
                speedLeft = speedRight = args[0]
                self.setMotorVel(speedLeft, speedRight)
                
            elif len(args) == 2:
                speedLeft = args[0]
                speedRight = args[1]
                self.setMotorVel(speedLeft, speedRight)

            else:
                print "Please enter arguments 'setspeed [left] [right]' or 'setspeed [both]' between -1000 and 1000"
    
    def do_begin(self, inputString):
        if inputString[:3] == "COM" or inputString[:3] == "tty":    #check that the port name appears to be proper
            self.MC.startSerial(inputString)
        else:
            print "Please input the port name starting with 'COM*' or 'tty*'"

    def do_x (self, inputString):   #  'x' is fast to type and makes a good Estop command shortcut.
        self.MC.ESTOP()
        
    def do_estop (self, inputString):
        self.MC.ESTOP()
        
    def do_eresume (self, inputString):
        self.MC.ERESUME()

    def do_maxspeed (self, inputString):  # WIP  Use to limit the motor speed for slow speed trials.
        try:
            self.MC.maxSpeed = int(inputString)
            print "Max Speed set to +- " + inputString
        except(exception):
            print "error in setMaxSpeed"


    # COMMAND DOCUMENTATION ############################################################################################
    def help_setspeed (self):
        print "\tThe motor controller accepts values between -1000 and 1000 for relative motor speed on each channel (left or right). \n\tEnter 'setspeed xx' or 'setspeed ll rr', where xx is the speed for both motors, and ll and rr are speeds for each motor. \n"
        
    def help_estop (self):
        print "Enables the Emergency Shutdown on the Motor Controller and can be resumed with 'eresume'.  \nThis command takes no arguments.\n"

    def help_x (self):  #This is just a shortcut to the estop command, so it shares the help file.
        self.help_estop ()
    
    def help_eresume (self):
        print "Instructs the Motor Controller to resume its function after an ESTOP command has been issued. \nThis command takes no arguments.\n"

if __name__ == "__main__":
    TB = TestBench()
    TB.cmdloop("Enter Commands")
    
# EOF #######################################################################

    
    
        
