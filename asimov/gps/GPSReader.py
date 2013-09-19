#!/usr/bin/python
#This is TECHBOT's GPS interface module.  It uses both GGA and VTG

import serial, math, time, sys

gpsTimeout = 3
gpsBaud = 4800

COOKEVILLE_TN_LAT_TO_M = 108687.824 #meters per degree lat, calculated HGM & TWH 4/12/09
COOKEVILLE_TN_LON_TO_M = 90924.650 #meters per degree lon, calculated HGM & TWH 4/12/09

#ROCHESTER_MI_LAT_TO_M = 111086.74 #meters per degree lat
#ROCHESTER_MI_LON_TO_M = 81962.81 #meters per degree lon

ROCHESTER_MI_LAT_TO_M = 111192.178608
ROCHESTER_MI_LON_TO_M = 84143.1095459 #meters per degree

TIME_ZONE_CHANGE = (-5) #add this to Coordinated Universal Time to obtain local time... Cookeville is (CUT - 5) hrs when on daylight savings.

class GPSClass(object):
    def __init__(self, port='/dev/ttyUSB1', verbose=True):
        self.verbose = verbose

        if port == 'read_from_file':
            portFile = open('/etc/usb_ports.txt', 'r')
            line = portFile.readline()

            while line.find('gpsPort') != 0:
                line = portFile.readline()
                print "Finding gpsPort..."

            self.gpsPort = line.split()[-1]

        else: self.gpsPort = port

        print 'GPS:', port, gpsBaud
        self.connGPS = serial.Serial(port = self.gpsPort, baudrate = gpsBaud, timeout = gpsTimeout)

        if self.connGPS.isOpen():
            print "GPSReader: Serial connection to GPS successful\n"
        else: print "GPSReader: Serial connection to GPS failed\n"

        print "GPSReader: Configured for ROCHESTER MI"
        self.lat2met = ROCHESTER_MI_LAT_TO_M
        self.lon2met = ROCHESTER_MI_LON_TO_M

        self.orientation = 0.0 #in radians

        #print "GPSReader: Configured for COOKEVILLE TN"
        #self.lat2met = COOKEVILLE_TN_LAT_TO_M
        #self.lon2met = COOKEVILLE_TN_LON_TO_M

    def getLat2met(self):
        return self.lat2met

    def getLon2met(self):
        return self.lon2met

    def getReading(self): #Since all data strings begin with $, are comma delimited, and end with *, parses the data strings
        byte = 'blank'
        i = 0

        self.connGPS.flushInput()
        while byte != '$':
            byte = self.connGPS.read(1)
            i += 1
            if byte == '' and self.verbose and i > 30:
                print "GPSReader: No data on serial line, trying again... (try ", i-30, ")"

        readstr = ""
        for i in range(100):
            byte = self.connGPS.read(1)
            if byte == '*': #end of a reading
                break
            readstr += byte
        reading_list = readstr.split(',')
        reading_list = reading_list[:-1] #cuts off null at end
        return reading_list


    def printUnformattedOutput(self, reading_list): #A useful function to just output the raw data without formatting
        print "GPSReader: UNFORMATTED:"
        print reading_list
        print ""


    def decode(self, reading_list): #decodes the raw data from the reading and appropriately casts it to floats or strings
        if reading_list[0] == 'GPGGA':
            if type(reading_list[1]) != str or reading_list[1] == '':
                time = 'ERROR'
            else:
                adjusted_time = str(int(reading_list[1][:2]) + TIME_ZONE_CHANGE) + reading_list[1][2:]
                time = float(adjusted_time[4:]) + (60)*float(adjusted_time[2:4]) + 3600*float(adjusted_time[:2]) #seconds

                #time = float(reading_list[1]) #If you want the time to come through without any formatting

            if type(reading_list[2]) != str or reading_list[2] == '':
                latitude = 'ERROR'
            else:
                latitude = int(reading_list[2][:2]) + (1./60)*float(reading_list[2][2:]) #deg
                if reading_list[3] == 'S':
                    latitude = -latitude

            if type(reading_list[4]) != str or reading_list[4] == '':
                longitude = 'ERROR'
            else:
                longitude = int(reading_list[4][:3]) + (1./60)*float(reading_list[4][3:]) #deg
                if reading_list[5] == 'W':
                    longitude = -longitude

            if reading_list[6] == '1':
                corr_type = 'SPS'
            elif reading_list[6] == '2':
                corr_type = 'DGPS'
            elif reading_list[6] == '3':
                corr_type = 'PPS'
            elif reading_list[6] == '4':
                corr_type = 'RTK'
            elif reading_list[6] == '5':
                corr_type = 'float RTK'
            elif reading_list[6] == '6':
                corr_type = 'EST'
            elif reading_list[6] == '7':
                corr_type = 'MANUAL'
            elif reading_list[6] == '8':
                corr_type = 'SIMULATION'
            else:
                corr_type = 'NO FIX'

            if type(reading_list[8]) != str or reading_list[8] == '':
                HDOP = 'ERROR'
            else:
                HDOP = float(reading_list[8])

            return [reading_list[0], time, latitude, longitude, corr_type, HDOP]

        elif reading_list[0] == 'GPVTG':
            track_true = reading_list[1]
            speed_kph = reading_list[7]
            if type(reading_list[7]) != str:
                speed = float(str(reading_list[7]))*1000./3600 #convert to m/s
            else:
                speed = 0.0

            if speed >= .15:
                self.orientation = float(track_true) * math.pi/180 #self.orientation is a global variable
                #print "\nOrientation set to", self.orientation
            else:
                pass
                #print "\nOrientation unchanged from", self.orientation
            return [reading_list[0], track_true, speed]

        elif reading_list[0] == 'GGA+VTG':
            pass #string is already formatted

        else:
            print "GPSReader: UNKNOWN GPS STRING TYPE"
            print "String =", reading_list
            return "UNKNOWN GPS STRING TYPE"


    def combineGPS(self, partial1, partial2): #Combines a VTG and GGA reading into a "GGA+VTG" hybrid
        if partial1[0] == 'GPGGA':
            if partial2[0] == 'GPVTG':
                fullReading = ['GGA+VTG'] + partial1[1:] + partial2[1:]
                return fullReading
        if partial1[0] == 'GPVTG':
            if partial2[0] == 'GPGGA':
                fullReading = ['GGA+VTG'] + partial2[1:] + partial1[1:]
                return fullReading

        #if code hasn't exited by now
        print "GPSReader: Unable to combine GPS strings, need a GPGGA and a GPVTG"
        return None


    def output(self, gpsdata): #Prints formatted GPS information to the screen
        print "GPSReader: FORMATTED:"
        if gpsdata == None:
            print "GPSReader does not know how to combine these strings"
            return None
        if gpsdata[0] == 'GPGGA' and len(gpsdata) == 5:
            print "Time [sec]       =", gpsdata[1]
            time_hms = self.time_as_hr_min_sec(gpsdata[1])
            print "Time [h:m:s]     =", time_hms[0], ":", time_hms[1], ":", time_hms[2]
            print "Latitude [deg]   =", gpsdata[2]
            print "Latitude [d,m,s] =", self.convertToDMS(gpsdata[2])
            print "Longitude [deg]  =", gpsdata[3]
            print "Longitude [d,m,s]=", self.convertToDMS(gpsdata[3])
            print "Correction Type  =", gpsdata[4]
            print "HDOP             =", gpsdata[5]
            print ""
        if gpsdata[0] == 'GPVTG':
            print "Track (True)[deg]=", gpsdata[1]
            print "Ground Speed[kph]=", gpsdata[2]
            print ""
        if gpsdata[0] == 'GGA+VTG':
            print "Time [sec]       =", gpsdata[1]
            time_hms = self.time_as_hr_min_sec(gpsdata[1])
            print "Time [h:m:s]     =", time_hms[0], ":", time_hms[1], ":", time_hms[2]
            print "Latitude [deg]   =", gpsdata[2]
            print "Latitude [d,m,s] =", self.convertToDMS(gpsdata[2])
            print "Longitude [deg]  =", gpsdata[3]
            print "Longitude [d,m,s]=", self.convertToDMS(gpsdata[3])
            print "Correction Type  =", gpsdata[4]
            print "HDOP [unitless]  =", gpsdata[5]
            print "Track(True) [deg]=", gpsdata[6]
            if gpsdata[7] != '':
                print "Ground Spd [m/s] =", gpsdata[7]
                print "Ground Spd [kph] =", float(str(gpsdata[7]))/1000.*3600
                print "Ground Spd [mph] =", (float(str(gpsdata[7]))/1000.*3600)*0.621371192
            else:
                print "No Speed Info Available"
            print ""
        return None

    def refreshPort(self): #Refreshes the serial port to clear old GPS strings
        print "GPSReader: Refreshing Serial Port..."
        self.closePort()
        self.connGPS = serial.Serial(port = self.gpsPort, baudrate = gpsBaud, timeout = gpsTimeout)
        if self.connGPS.isOpen():
            print "GPSReader: Connection to GPS refreshed\n"
        else:
            print "GPSReader: Serial connection to GPS failed\n"
        return None


    def closePort(self): #Closes the gps port, only necessary because "connGPS" is not a global variable
        self.connGPS.close()

    def time_as_hr_min_sec(self, time_in_seconds):
        if time_in_seconds == 'ERROR':
            return ('X', 'X', 'X')

        seconds = int(time_in_seconds)

        hours = seconds / 3600
        minutes = (seconds % 3600) / 60
        seconds = ((seconds%3600) % 60)

        return [hours, minutes, seconds]


    def convertToDMS(self, reading_in_degrees):
        if reading_in_degrees == 'ERROR':
            return ('X','X','X')

        reading_in_seconds = reading_in_degrees * 3600

        degrees = int(reading_in_degrees)
        minutes = int((reading_in_degrees - degrees) * 60)
        seconds = (((reading_in_degrees - degrees) * 60) - minutes) * 60
        seconds = round(seconds, 6) #rounds to the nearest millionth of a second

        return[degrees, minutes, seconds]


#End of GPS Class



def getXYDist(gps_instantiation, coord1 = None, coord2 = None):
    '''This code converts gps waypoints into x and y components based on compass heading'''
    if coord1 is None:
        coord1 = getFormattedGPS(gps_instantiation)[2:4] + [0]

    #orientation = compass.getBearing() * (math.pi/180)
    # Orientation is now a global variable, which represents the last valid track_true

    latDist = coord2[0] - coord1[0]
    lonDist = coord2[1] - coord1[1]

    latMeters = latDist * gps_instantiation.lat2met
    lonMeters = lonDist * gps_instantiation.lon2met
    # There are two tricks here.  The first is that compasses measure pos
    # thetas CW.  The second is that a pos rotation is like rotating
    # the map negatively underneath you.  The net result is a double negative,
    # and the sign of orientation stays the same
    s = math.sin(gps_instantiation.orientation)
    c = math.cos(gps_instantiation.orientation)

    xDist = c*latMeters + -s*lonMeters
    yDist = s*latMeters + c*lonMeters

    return [xDist, yDist] #Distances to the goal relative to current pos and orient



def getAverageReading(gps_instantiation, number_of_readings, verbose = True):

    if verbose: print "Gathering the average of", number_of_readings, "readings"

    latTotal = 0
    lonTotal = 0

    for i in range(number_of_readings):
        reading = getFormattedGPS(gps_instantiation)
        while reading[2] == 'ERROR' or reading[3] == 'ERROR':
            print "Lost GPS satellites, retrying..."
            reading = getFormattedGPS(gps_instantiation)
        latTotal += reading[2]
        lonTotal += reading[3]
        if verbose: print "Reading", i+1, "of", number_of_readings, "gathered"

    averageLat = latTotal / number_of_readings
    averageLon = lonTotal / number_of_readings

    return [averageLat, averageLon]



def getFormattedGPS(gps_instantiation):

    # Loop, taking readings and checking to see if they're GGA or VTG
    raw1 = ["blank"]
    i = 0

    while raw1[0] != 'GPGGA' and raw1[0] != 'GPVTG': #makes raw1 read until either GPGGA or GPVTG
        raw1 = gps_instantiation.getReading()
        if raw1[0] == 'GPGGA': #sets raw2_needed to the "other" one (GGA or VTG)
            raw2_needed = 'GPVTG'
        else: raw2_needed = 'GPGGA'
        i += 1
        if i > 40: #if we havent gotten something by NOW, something went wrong
            print "GPSReader: Error occurred in getFormattedGPS, loop ran too long"
            return None


    raw2 = ["blank"]
    i = 0

    while raw2[0] != raw2_needed: #makes raw2 be the "other" one
        raw2 = gps_instantiation.getReading()
        i += 1
        if i > 40:
            print "GPSReader: Error occurred in getFormattedGPS, loop ran too long"
            return None

    read1 = gps_instantiation.decode(raw1)
    read2 = gps_instantiation.decode(raw2)
    totalReading = gps_instantiation.combineGPS(read1, read2)
    return totalReading




def calibrateGPS(gps_instantiation): #This method is the calibration routine.

    #The method works as follows:
    #Step 1: Take a few initial readings to ensure that the GPS is connected
    #Step 2: Take a number of readings to get the average latitude and longitude (Reading 1)
    #Step 3: Instruct the user to move North or South and input the exact distance
    #Step 4: Take a number of readings to get the average lat and lon (Reading 2)
    #Step 5: Divide the distance traveled by the lattitude difference to get a [meters/degree] conversion factor for latitude
    #Step 6: Repeat step 2 (Reading 3)
    #Step 7: Repeat step 3, except instruct to move East or West
    #Step 8: Repeat step 4 (Reading 4)
    #Step 9: Repeat step 5 for Longitude
    #Step 10: Output all the data for the user.

    print "GPSReader: Preparing to gather GPS data"

    trial = 3
    gather = 10

    #Test the GPS with 3 trial readings
    for attempt in range(trial):
        prep = getFormattedGPS(gps)
        if prep == None:
            print "*"*5, "Unable to gather GPS data", "*"*5
            return None
        else:
            print attempt+1, "of", trial, "successful"
            print "\nData:", prep

    print "\nGPSReader: Preparation Complete\n"

    #Gather Initial data
    print "*"*5, "Gathering Data, DO NOT MOVE", "*"*5


    #Reading 1, initial position for measuring Latitude change
    avgReading = getAverageReading(gps_instantiation=gps, number_of_readings=gather, verbose=True)

    averageLat1 = avgReading[0]
    averageLon1 = avgReading[1]

    print "\nData Gather  Complete"
    print "Average Lat:", averageLat1
    print "Average Lon:", averageLon1
    print ""

    #Move N/S to measure latitude change from reading 1 to reading 2
    vertDist = raw_input("\n\nMove me North or South and enter the distance [m]: ")


    gps_instantiation.refreshPort() #Clear the output buffer


    #Reading 2, final position for measuring Latitude change
    avgReading = getAverageReading(gps_instantiation=gps, number_of_readings=gather, verbose=True)

    averageLat2 = avgReading[0]
    averageLon2 = avgReading[1]

    deltaLat = averageLat2 - averageLat1
    latToMeters = float(vertDist)/deltaLat

    errorLon = abs(averageLon2 - averageLon1)

    #Allow user to turn robot before gathering new start data
    print "Now, turn me to face E/W"
    raw_input("When ready to capture GPS data, press Enter...")
    gps_instantiation.refreshPort()

    #Re-localize coordinates
    print "*"*5, "Gathering Data, DO NOT MOVE", "*"*5

    latTotal3 = 0
    lonTotal3 = 0

    #Reading 3, Initial position for measuring longitude.  Could be same or different from Reading 2 position.
    avgReading = getAverageReading(gps_instantiation=gps, number_of_readings=gather, verbose=True)

    averageLat3 = avgReading[0]
    averageLon3 = avgReading[1]

    print "\nData Gather Complete"
    print "Average Lat:", averageLat3
    print "Average Lon:", averageLon3
    print ""

    #Move E/W to measure longitude change
    horizDist = raw_input("Move me East or West and enter the distance [m]: ")
    gps_instantiation.refreshPort()


    #Reading 4, final position for measuring longitude.
    avgReading = getAverageReading(gps_instantiation=gps, number_of_readings=gather, verbose=True)

    averageLat4 = avgReading[0]
    averageLon4 = avgReading[1]

    deltaLon = averageLon4 - averageLon3
    lonToMeters = float(horizDist)/deltaLon

    errorLat = averageLat4 - averageLat3

    print "\n\nData points used for calibration:\n"
    print "Point 1:", averageLat1, averageLon1
    print "    Distance from 1 to 2:", vertDist
    print "Point 2:", averageLat2, averageLon2
    print
    print "Point 3:", averageLat3, averageLon3
    print "    Distance from 3 to 4:", horizDist
    print "Point 4:", averageLat4, averageLon4

    #Output total calibration results
    print "\n\n\n\n", "*"*5, "RESULTS", "*"*5
    print "Latitude change (potential for error):", errorLat
    print "Meters per degree Latitude:", latToMeters
    print "Longitude change: (potential for error)", errorLon
    print "Meters per degree Longitude:", lonToMeters

    raw_input("\n\n\nPress Enter to continue...")


def liveFromReferencePoint(gps_instantiation):

    gather = 5

    raw_input("When ready to set your reference point, press Enter...")

    avgReading = getAverageReading(gps_instantiation=gps, number_of_readings=gather, verbose=True)

    lat_0 = avgReading[0]
    lon_0 = avgReading[1]

    print "\nStarting Coordinates:"
    print "Latitude =", lat_0
    print "Longitude=", lon_0

    continue_flag = 0
    while continue_flag != 'q':
        gpsReading = getFormattedGPS(gps_instantiation)
        current_lat = gpsReading[2]
        current_lon = gpsReading[3]

        delta_lat = lat_0 - current_lat
        delta_lon = lon_0 - current_lon

        delta_lat_meters = delta_lat * gps_instantiation.lat2met
        delta_lon_meters = delta_lon * gps_instantiation.lon2met

        print "Latitude [deg]   =", current_lat
        print "Latitude [d,m,s] =", gps_instantiation.convertToDMS(current_lat)
        print "Longitude [deg]  =", current_lon
        print "Longitude [d,m,s]=", gps_instantiation.convertToDMS(current_lon)
        print
        print "Change in Lat [deg]  =", delta_lat
        print "Change in Lat [d,m,s]=", gps_instantiation.convertToDMS(delta_lat)
        print "Change in Lon [deg]  =", delta_lon
        print "Change in Lon [d,m,s]=", gps_instantiation.convertToDMS(delta_lon)
        print
        print "Meters Lat [m] =", delta_lat_meters
        print "Meters Lon [m] =", delta_lon_meters
        print
        continue_flag = raw_input("Type in 'q' to quit, or press Enter to take another reading...")

    print "liveOutput: Code finished executing"

def continualOutput(gps_instantiation):
    start_time = time.time()

    for j in range(10):
        full = getFormattedGPS(gps)
        gps.output(full)
        print "GPSReader: iteration =", j+1
        #print "Current time = ", time.time() - start_time
        #gps.printUnformattedOutput(gps.getReading())


####GPS Constant output code
if __name__ == '__main__':
    gps = GPSClass()

    continualOutput(gps)
    #liveFromReferencePoint(gps)
    #calibrateGPS(gps)

    #print getAverageReading(gps, 10)

    gps.closePort()

    if gps.connGPS.isOpen():
        print ""
        print "GPS Port NOT closed!"
    else:
        print ""
        print "GPS Port closed"
