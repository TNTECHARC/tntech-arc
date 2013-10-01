#!/usr/bin/python

from ctypes import *
#from pylab import *
import math, time, serial, sys, random, os, struct, checksum
from threading import Thread
import threading

class Ranger(object):

    def __init__(self, verbose=True, timeout=.5, port = '/dev/ttyUSB0'):
        ''' Set up the SICK. '''

        self.verbose = verbose
        self.conn = '' # set conn to (empty) before opening port
        self.timeout = timeout
        self.scan = None

        self.baudRate = 38400

        self.max_ack_time = .06 #seconds, this is maximum time for SICK to respond to command with ACK
                           # determined experimentally by "test_response_time()" method

        self.max_sleep_time = .15 #seconds.  This is the maximum sleep time between sending commands

        self.sickPort = port

        self.t1_stop = threading.Event()
        self.recvThread = Thread(target=self.recvLoop, args=( 1, self.t1_stop ) )

    def openPort(self, verbose = True):
        '''This method opens the SICK port.
           As a confirmation that the SICK is talking, it ALWAYS outputs some of the SICK's status

           Returns True if successful, and False if not.
        '''
        self.conn = serial.Serial(port=self.sickPort,
                                      baudrate=self.baudRate,
                                      bytesize=serial.EIGHTBITS,
                                      stopbits=serial.STOPBITS_ONE,
                                      timeout=self.timeout)

        if self.verbose and self.conn.isOpen():
                print "SICK: Port " + str(self.sickPort) + " opened successfully"


        #initial loop conditions
        starting_time = time.time()
        leave_connection_loop = False
        max_time_to_attempt_connection = 10 #seconds
        max_attempts = 10

        while leave_connection_loop == False:

            #This is a timer which causes loop to end after XX seconds of trying
            if (time.time() - starting_time) > max_time_to_attempt_connection:
                print "Unable to connect within the time limit"
                return False


            if self.conn.inWaiting() != 0: #the port is being flooded with data
                if self.verbose:
                    print "SICK appears to be in 'Continuous Monitor Mode'"

                attempt = 1
                while self.setSickResponseMode() == False and attempt <= max_attempts:
                    print "Retrying 'Set to Response Mode'... attempt", attempt, "of", max_attempts
                    time.sleep(random.random() * self.max_sleep_time)
                    attempt += 1

                if attempt > max_attempts:
                    self.conn.close()
                    while self.openPort() != True:
                        print "Trying to open port again, might be infinite loop"


            print "Requesting Error Status at", self.baudRate, "baud"
            self.writeString(self.formTelegram(command = '\x32'))

            time.sleep(self.max_ack_time)

            if self.readString(1) == '\x06':
                start_of_response = self.readString(4) #determine the length of the response
                length_of_response = struct.unpack('<1H', start_of_response[2:4])[0]
                message = self.readString(length_of_response)
                if message[-1] == '\x10':
                    print "Current Error Status: OK"
                else:
                    print "Current Error Status: NOT OK (", hex(ord(message[-1])), ")"
                chksum = self.readString(2)

                leave_connection_loop = True

            if leave_connection_loop == False: #if still equal, try again
                print "Retrying attempt to establish connection with SICK, elapsed time =", round(time.time()-starting_time, 1), "of", max_time_to_attempt_connection, "seconds"

        #if you got outside the while loop without hitting the "return false" then
        #SICK and computer established a connection
        return True

    def closePort(self, verbose = True):
        self.conn.close()
        if not self.conn.isOpen() and verbose:
            print "Sick connection closed successfully!"
        elif self.conn.isOpen() and verbose:
            print "SICK IS STILL OPEN"

    def writeString(self, string):
        ''' Write directly to serial port '''

        #print "WRITING TO PORT:\n", [hex(ord(x)) for x in string]

        self.conn.write(string)

    def readString(self, numRead = 'all'):
        ''' Read directly from serial port '''

        #reading_string = self.conn.read(numRead)
        #print "READING FROM PORT:\n", [hex(ord(x)) for x in reading_string]
        #return reading_string
        if numRead == 'all':
            return self.conn.read(self.conn.inWaiting())

        return self.conn.read(numRead)

    def flushPort(self):
        ''' Flush all data coming from the SICK '''
        self.conn.read(self.conn.inWaiting())
        self.conn.flushOutput()
        self.conn.flushInput() #this should be redundant, but just making sure

    def formTelegram(self, command, data = ''):
        # This method takes care of adding the STX, Address, Length, and Checksum to any command, and returns the telegram string

        STX = '\x02'
        address = '\x00'

        length = struct.pack('<i', len(command + data))
        length = length[0:2] #a python int has four bytes, we only want to transmit two...

        message = STX + address + length + command + data

        csum = checksum.calcChecksum(message)
        csum = struct.pack('<i', csum)
        csum = csum[0:2] #only want first two bytes of the 4-byte integer

        return message + csum


    def getSickStatus(self):
        checksum_confirmed = False

        while checksum_confirmed == False:
            self.writeString(self.formTelegram(command = "\x31"))

            status_telegram = self.readString(161)

            if len(status_telegram) == 161:
                ack = status_telegram[0]
                stx = status_telegram[1]
                adr = status_telegram[2]
                length = status_telegram[3:5]
                response = status_telegram[5]
                data = status_telegram[6:159]
                chksum = status_telegram[159:161]
                calculated_checksum = struct.pack('<i', checksum.calcChecksum(stx + adr + length + response + data))

                if chksum[0] == calculated_checksum[0] and chksum[1] == calculated_checksum[1]:
                    checksum_confirmed = True
                else:
                    checksum_confirmed = False
                    print "SICK.getSickStatus: checksum doesn't confirm, retrying..."

            else:
                "****len was not 161"

        status_string = self.interpretSickStatus(data)

        return status_string

    def interpretSickStatus(self, responseData):


        status_string = ''

        #IMPORTANT NOTE: The following code outputs certain parts of the response
        #                string.  The structure comes from the SICK Telegram book
        #                starting on Page 52, ALTHOUGH THE PLACEMENT DOES NOT MATCH!
        #                For example, items 115 and 116 (which are the 116th and 117th bytes)
        #                appear to tell the baudrate, although the book implies these
        #                should be found on the 110th and 111th.  I don't know why
        #                the manual and the SICK disagree, but the manual has a few
        #                small errors (misstypes) so I trust the SICK ranger.
        #
        #ANOTHER NOTE:  There is more data available in this configuration, if we want
        #               to check for other "important data" which I chose not to output.

        status_string = status_string + '\n' +  "\n" + "*" * 15
        status_string = status_string + '\n' +  "SICK RESPONDED WITH THE FOLLOWING DATA:\n"
        status_string = status_string + '\n' +  "Software Version =     " + str(responseData[0] + \
                                                                                responseData[1] + \
                                                                                responseData[2] + \
                                                                                responseData[3] + \
                                                                                responseData[4] + \
                                                                                responseData[5] + \
                                                                                responseData[6])

        status_string = status_string + '\n' +  "Operating Mode =       " + hex(ord(responseData[7]))

        if responseData[8] == '\x00':
            errorIndicator = "No Error"
        else:
            errorIndicator = "ERROR PRESENT! CODE:" + hex(ord(responseData[8]))
        status_string = status_string + '\n' +  "Error Indicator =      " + errorIndicator

        scanAngle = struct.unpack('<1H', responseData[106]+responseData[107])[0]
        status_string = status_string + '\n' +  "Scan Angle =           " + str(scanAngle) + " [degrees]"

        angleRes = struct.unpack('<1H', responseData[108]+responseData[109])[0]
        status_string = status_string + '\n' +  "Angle Resolution =     " + str(angleRes) + " [100ths of a degree]"

        baudRate = responseData[116] + responseData[115]
        if baudRate == '\x80\x01':
            baudRate = "500000 (according to SICK)"
        elif baudRate == '\x80\x67':
            baudRate = "9600 (according to SICK)"
        elif baudRate == '\x80\x19':
            baudRate = "38400 (according to SICK)"
        elif baudRate == '\x80\x33':
            baudRate = "19200 (according to SICK)"
        else:
            baudRate = "UNKNOWN CODE: " + baudRate
        status_string = status_string + '\n' +  "Baud Rate =            " + baudRate

        units = responseData[121]
        if units == '\x00':
            units = 'cm'
        elif units == '\x01':
            units = 'mm'
        else:
            units = 'UNKNOWN UNITS CODE: ' + units
        status_string = status_string + '\n' +  "Units of measurement = " + units

        permanentBaudRate = responseData[118]
        if permanentBaudRate == '\x00':
            permanentBaudRate = "Baud Rate set to 9600 when SICK is restarted"
        elif permanentBaudRate == '\x01':
            permanentBaudRate = "Baud Rate " + baudRate + " will be RETAINED when the SICK is restarted"
        else:
            permanentBaudRate = "UNKNOWN BAUDRATE CODE: " + permanentBaudRate
        status_string = status_string + '\n' +  permanentBaudRate

        status_string = status_string + '\n'*2

        return status_string


    def setSickContinuousMonitorMode(self):
        '''Set SICK mode: continuously output range scans'''
        self.writeString(self.formTelegram(command = '\x20', data = '\x24'))

        time.sleep(self.max_ack_time)

        ack_byte = self.readString(1)

        if ack_byte != '\x06':
            if ack_byte == '':
                print "Sick: Responded with NOTHING to 'Set Continuous Monitor Mode'"
            else:
                print "SICK: Responded", hex(ord(ack_byte)), "to 'Set Continuous Monitor Mode'"
            return False

        else:
            response = self.readString(9)
            if response[5] == '\x00':
                return True
            else:
                print "Sick responded: 'Mode =", hex(ord(response[5])), "to 'Set Continuous Mode'"
                print "Either there's an error in the SICK, or you have incorrect password"
                return False


    def setSickResponseMode(self):
        ''' Set SICK mode: respond to commands'''

        self.writeString(self.formTelegram(command = '\x20', data = '\x25'))

        time.sleep(self.max_ack_time * 3) #give SICK time to stop spamming

        #print "in waiting =", self.conn.inWaiting()
        response = self.readString('all')
        #for x in response:
        #    print hex(ord(x)),
        print

        last_nine = response[-9:]

        if len(last_nine) != 9:
            print "There are not nine bits on the line"
            return False

        elif last_nine[4] == '\xA0':
            if last_nine[5] == '\x00':
                return True
            elif last_nine[0:7] == '\x02\x80\x03\x00\xa0\x01\x10':
                print "Sick Responded: 'Mode =", hex(ord(last_nine[5])), "to 'Set Response Mode'"
                print "Either there's an error in the SICK, or you have incorrect password"
                self.flushPort()
                return False
            else:
                #Got lucky to hit \xA0, not part of response.
                self.flushPort()
                print self.readString('all')
                return False
        else:
            #last nine are just part of the continuous output
            self.flushPort()
            print self.readString('all')
            return False


    def isInResponseMode(self):

        time.sleep(.1)

        if self.conn.inWaiting() == 0:
            return True
        else:
            return False


    def resetCommand(self):
        ''' Reset SICK configuration to 9600 baud, non-monitor mode.
            Reponse should be:
            06 - ack
            0280 - stx & address
            0200 - length bytes
            91 - response
            1079 3002 8017
            0090 4C4D 5332
            3931 3B53 3035
            3036 353B 5830
            312E 3230 20 - message
            10 - LMS status
            FC34 - checksum
        '''
        #self.conn.write("\x02\x00\x01\x00\x10\x34\x12") #reset yourself
        self.conn.write(self.formTelegram(command = '\x10'))
        returnString = self.conn.read(38)
        #print "length of string", len(returnString)

        if self.verbose: print "reset Sick:", repr(returnString)


    def recvLoop(self, kwargs, stop_event):
        ''' Thread to probe SICK for coords as fast as it can read them.
            Not to be called directly!

            Each scan starts with the same 7 byte sequence.
            We need to keep reading from the sick connection until we see the
            starting bytes. We can then read a full sweep (362 bytes) and store
            in the scan array.
        '''

        startBytes = ['\x02','\x80','\x6E','\x01','\xB0','\xB5','\x40']
        next = 0
        while not stop_event.is_set():
            byte = self.conn.read(1)
            while byte == startBytes[next]:
                byte = self.conn.read(1)
                next += 1
                if next == len(startBytes) - 1:
                    s = self.conn.read(362)
                    self.conn.read(3) # last 3 bytes are not part of the scan
                    s = struct.unpack('<181H', s)
                    self.scan = [s[i]/(1000.0) for i in range(181)]
                    next = 0
                    break

    def startRecvThread(self):
        ''' Start collecting SICK data. '''
        self.recvThread.start()
        time.sleep(.5)

    def closeRecvThread(self):
        ''' Currently has no use. Thread cannot be stopped peacefully. '''
        #self.recvThread.join()
        self.t1_stop.set()

    def getScan(self):
        ''' Request for data when not in monitor mode and for 9600 baud '''
        self.conn.write("\x02\x00\x02\x00\x30\x01\x31\x18") # 9600 baud
        scan = self.conn.read(373)
        print scan
        print len(scan)
        print len(scan[8:370])
        if len(scan) < 373:
            print "failed scan!"
            return []
        s = struct.unpack('<181H', scan[8:370])
        return [s[i]/(1000.) for i in range(181)]

    def setSickBaudrate(self, new_baud=9600):
        ''' Set SICK to new_baud.
        Reponse should be:
        06 - ack
        0280 - stx & address
        0300 - length bytes
        A0 - response
        00 - message
        10 - LMS status
        160A - checksum
        '''
        baud_dictionary = {9600:"\x42", 19200:"\x41", 38400:"\x40", 500000:"\x48"}

        self.setTimeout(.5)

        baud_hex_code = baud_dictionary[new_baud]
        telegram = self.formTelegram(command = '\x20', data = baud_hex_code)

        while self.conn.inWaiting() != 0:
            self.flushPort()

        returnString = ''
        counter = 0
        while returnString != "\x06\x02\x80\x03\x00\xA0\x00\x10\x16\x0A":
            if counter > 0:
                print "SICK did not confirm baud switch, trying again"
                self.writeString(telegram)
                time.sleep(.1)
                returnString = self.readString(self.conn.inWaiting())

            if counter > 5:
                print "SICK not confirming baud switch, attempt number", counter

            counter += 1

        #print "length:", len(returnString)
        if self.verbose:
            print "SICK responded: ", [hex(ord(x)) for x in returnString]

        self.setTimeout(self.timeout)
        if self.verbose:
            print "SICK Timeout set to " + str(self.timeout)

        self.setSerialBaudrate(new_baud)
        if self.verbose:
            print "SICK Serial Baudrate set to", self.BaudrateMethods.getSerialBaudrate()

    def setSerialBaudrate(self, new_baud=9600, verbose=False):
        self.conn.setBaudrate(new_baud)
        if verbose: print "SICK serial connection baudrate set to", new_baud

    def getSerialBaudrate(self):
        return self.conn.baudrate

    def setTimeout(self, timeout=-2):
        if timeout == -2:
            self.conn.timeout = self.timeout
        else:
            self.conn.timeout = timeout
            self.timeout = timeout


##End of Ranger class