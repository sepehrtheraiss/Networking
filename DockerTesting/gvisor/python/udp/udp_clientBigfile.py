import socket
import sys
import time

server_addr = ('172.17.0.1',8082)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
    time.sleep(0.5)
    msg = "client msg"
    sent = sock.sendto(msg,server_addr)
    print >>sys.stderr, 'sent %s of %s bytes' %(msg,len(msg))
    print >>sys.stderr, 'waiting'
    data, server = sock.recvfrom(256)
    print >>sys.stderr, 'recvived %s' %(data)
    time.sleep(0.5)

    size = int(data)
    total = 0
    while(total < size):
        data , server = sock.recvfrom(2056)
        total += len(data)
        print "read %s bytes" %total
    print "\nread %s of %s bytes" %(total, size)
except Exception as e:
    print >>sys.stderr,e

print >>sys.stderr, 'closing\n\n'
sock.close()
