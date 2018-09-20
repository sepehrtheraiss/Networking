import socket
import sys
import time

server_addr = ('172.17.0.1',8081)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
msg = ''
time.sleep(0.5)

while msg != 'exit':
    try:
        msg = raw_input('\nmsg: ')
        sent = sock.sendto(msg,server_addr)
        print >>sys.stderr, 'sent %s of %s bytes' %(msg,len(msg))
        print >>sys.stderr, 'waiting'
        data, server = sock.recvfrom(256)
        print >>sys.stderr, 'recvived %s bytes' %len((data))
    except Exception as e:
        print >>sys.stderr,e

print >>sys.stderr, 'closing\n\n'
sock.close()
