import socket
import sys
import time

s_addr = ('0.0.0.0', 8080)
msg = 'udp server message'

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.bind(s_addr)

while True:
    print >>sys.stderr, '\nwaiting to recevive message'
    data, addr = sock.recvfrom(4096)
    print >>sys.stderr, 'received %s bytes from %s' %(len(data),addr)
    print >>sys.stderr, data
   # f = open("bigfile","r")
   # msg = f.read()
   # f.close()
    if data:
        try:
#          print >>sys.stderr, 'will send udp packet in 3 seconds'
#            time.sleep(3)
           # total = 0
          #  n = 0
            print >>sys.stderr, 'msg size %s' %(len(msg))
           # while(total < len(msg)):
            #    total += 512
             #   if total > len(msg):
              #      total = len(msg)
                #send = sock.sendto(msg[n:total],addr)
            addr = ('1231231231',8080)
            send = sock.sendto(msg,addr)
               # n += 512
            print >>sys.stderr, 'sent %s bytes back to %s' %(send,addr)
        except Exception as e:
            print >>sys.stderr,e
