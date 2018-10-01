import socket
import sys
import time

s_addr = ('0.0.0.0', 8082)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(s_addr)

print >>sys.stderr, '\nwaiting to recevive message'
data, addr = sock.recvfrom(256)
print >>sys.stderr, 'received %s bytes from %s' %(len(data),addr)
print >>sys.stderr, data
f = open("bigfile","r")
msg = f.read()
f.close()
if data:
    try:
        size = str(len(msg))
        sock.sendto(size,addr)
        time.sleep(0.5)
        total = 0
        n = 0
        while(total < len(msg)):
            total += 512
            if total > len(msg):
                total = len(msg)
            send = sock.sendto(msg[n:total],addr)
            n += 512
        print >>sys.stderr, 'sent %s bytes back to %s' %(total,addr)
    except Exception as e:
        print >>sys.stderr,e
