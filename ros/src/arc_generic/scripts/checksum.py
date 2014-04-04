#!/usr/bin/python
def calcChecksum(data):
    ''' data needs to be a string of hex characters
    method returns a NUMBER, not a string'''
    
    data = list(data)
    data = [ord(x) for x in data]
    
    CRC16_GEN_POL = 0x8005
    data = [0x00] + data
    uCrc16 = 0
    for i in range(1,len(data)):
        if uCrc16 & 0x8000:
            uCrc16 = (uCrc16 & 0x7fff)<<1
            uCrc16 ^= CRC16_GEN_POL 
        else:
            uCrc16 <<= 1
        uCrc16 ^= data[i] | ((data[i-1]) << 8)

    return uCrc16

if __name__ == '__main__':
    comdata = '\x02\x00\x02\x00\x20\x40'
    print list(comdata)
    print 'Checksum is %x' % calcChecksum(comdata)
