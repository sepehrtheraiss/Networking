import socket
import sys
import os
#from subprocess import call

#call(["ifconfig"])
os.system('tcpdump -n -i eth0')

pid = os.fork()

if pid == 0:
    print >>sys.stderr, 'child process'
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    server_addr = ('0.0.0.0',8080)
    msg = 'client msg'

    try:
        import time
        time.sleep(2)
        print >>sys.stderr, 'sending %s' %msg
        sent = sock.sendto(msg,server_addr)
        
        print >>sys.stderr, 'waiting'
        data, server = sock.recvfrom(4096)
        print >>sys.stderr, 'recvived %s' %data

    finally:
        print >>sys.stderr, 'closing'
        sock.close()
else:
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s_addr = ('0.0.0.0', 8080)
    sock.bind(s_addr)

    while True:
        print >>sys.stderr, '\nwaiting to recevive message'
        data, addr = sock.recvfrom(4096)
        print >>sys.stderr, 'received %s bytes from %s' %(len(data),addr)
        print >>sys.stderr, data

        if data:
            msg = 'udp server message'
            try:
                send = sock.sendto(msg,addr)
                print >>sys.stderr, 'sent %s bytes back to %s' %(send,addr)
            except Exception as e:
                print >>sys.stderr,e

