from serial import Serial
import time
import numpy as np
import sys


#A word for word copy of the code from andros
class Hoky(object):
    def __init__(self, port='/dev/ttyACM0'):
        self.s = Serial(baudrate=19200, port=port)
        self.s.setTimeout(0.25)
        self.s.open()
        self.s.write('BM\n')
        self.s.read(8)


    def getAllLines(self):
        line = []
        keepGoing = True
        prevCh = ''
        ch = ''
        while keepGoing:
            ch = self.s.read(1)
            line.append(ch)
            if prevCh == '\n' and ch == '\n':
                keepGoing = False
            prevCh = ch
        return ''.join(line)

    def getScan(self):
        # three char encoded
        self.s.write('GD0044072500\n')
        retVal = self.getAllLines()
        retLines = retVal.splitlines()
        # first line is echo
        # second line is status
        # third line is timestamp
        # fourth+ line is data (65 bytes with sum byte)
        #print retLines
        scanVal = ''.join(x[:64] for x in retLines[3:])
        scanVal = [ord(ch)-(16*3) for ch in scanVal]
        scanDecoded = []
        prev = 20
        curr = 20
        for i in range(0,len(scanVal)-3,3):
            sv1 = scanVal[i+0]
            sv2 = scanVal[i+1]
            sv3 = scanVal[i+2]
            curr = (sv1<<12)+(sv2<<6)+sv3
            if curr <= 20: # bad numbers...
                curr = prev

            if curr < 20: print curr

            scanDecoded.append(prev)
            prev = curr

        scanDecoded.append(curr)
        # LATER: implement two pass algorithm discussed here:
        # http://tntechigvc.wikidot.com/forum/t-185605/programming-to-do-list#post-791386
        return np.array(scanDecoded[1:])

    def getPolarCoords(self):
        scan = self.getScan()
        if len(scan) == 0:
            print 'ERROR: NO SCAN!!'
            sys.exit(0)
            return
	print len(scan)
        resTheta = (240. / len(scan))/180. * 3.141592
	print resTheta
        thetaList = np.arange(len(scan))*resTheta-np.pi/6
        return thetaList, scan

    def getXYCoords(self):
        th, r = self.getPolarCoords()
        return r * np.cos(th), r * np.sin(th)

    def close(self):
        self.s.write('QT\n')
        self.s.close()

    def open(self):
        self.s.open()
        self.s.write('BM\n')
        self.s.read(8)


def plotRadar(hoky):
    import matplotlib
    matplotlib.use('GTKAgg') # do this before importing pylab
    import gobject
    import matplotlib.pyplot as plt

    # radar green, solid grid lines
    plt.rc('grid', color='#316931', linewidth=1, linestyle='-')
    plt.rc('xtick', labelsize=15)
    plt.rc('ytick', labelsize=15)

    size = 6
    fig = plt.figure(figsize=(size, size))
    ax = fig.add_axes([0.0, 0.0, 1.0, 1.0], polar=True, axisbg='#d5de9c')
    line, = plt.polar(*hoky.getPolarCoords())
    ax.grid(True)

    def update():
        line.set_data(*hoky.getPolarCoords())
        fig.canvas.draw_idle()
        return True

    gobject.timeout_add(100, update)  # you can also use idle_add to update when gtk is idle
    plt.show()

def plotCartesian(hoky):
    import matplotlib
    matplotlib.use('GTKAgg') # do this before importing pylab
    import gobject
    import matplotlib.pyplot as plt
    size = 6
    fig = plt.figure(figsize=(size, size))
    ax = fig.add_subplot(111)
    xData, yData = hoky.getXYCoords()
    line, = ax.plot(xData, yData, '.')

    def update():
        line.set_data(*hoky.getXYCoords())
        fig.canvas.draw_idle()
        return True  # return False to terminate the updates

    gobject.timeout_add(100, update)  # you can also use idle_add to update when gtk is idle
    plt.show()


def timeIt(hoky):
    start = time.time()
    while True:
        hoky.getPolarCoords()
        print 1. / (time.time()- start)
        start = time.time()


#test function to isolate various properties of the matplotlib
def graph():
   import matplotlib
   matplotlib.use('GTKAgg') # do this before importing pylab
   import gobject
   import matplotlib.pyplot as plt
   size = 6
   fig = plt.figure(figsize=(size, size))
   ax = fig.add_subplot(111)
   #xData, yData = hoky.getXYCoords()
   xData = [0.5, 1.0, 0.2, 0.4 ]
   yData = [0.3, 1.0, 0.2, 0.8 ]
   line, = ax.plot(xData, yData, '.')

   def update():
      line.set_data( [xData, yData ] )
      fig.canvas.draw_idle()
      return True  # return False to terminate the updates

   gobject.timeout_add(100, update)  # you can also use idle_add to update when gtk is idle
   plt.show()



if __name__ == '__main__':

    #graph()
    h = Hoky(port='/dev/ttyACM0')
    #timeIt(h)
    #plotCartesian(h)
    plotRadar(h)

