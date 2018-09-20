import socket
import sys
import time

s_addr = ('0.0.0.0', 8081)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(s_addr)
data = 'blank'
msg = 'udp server message'

print >>sys.stderr, '\nwaiting to recevive message'
data, addr = sock.recvfrom(256)
print >>sys.stderr, 'received %s bytes from %s' %(len(data),addr)
print >>sys.stderr, 'data: %s' %data
sock.connect(addr)

send = sock.send(msg)
print >>sys.stderr, 'sent %s bytes back to %s' %(send,addr)

while data != 'exit':
    if data:
        try:
            print >>sys.stderr, '\nwaiting to recevive message'
            data, addr = sock.recvfrom(256)
            print >>sys.stderr, 'received %s bytes from %s' %(len(data),addr)
            print >>sys.stderr, 'data: %s' %data

            send = sock.send(msg)
            print >>sys.stderr, 'sent %s bytes back to %s' %(send,addr)
        except Exception as e:
            print >>sys.stderr,e

print >>sys.stderr, 'closing\n\n'
sock.close()

