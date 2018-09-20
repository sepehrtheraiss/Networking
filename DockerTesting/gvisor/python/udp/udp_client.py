from os import system
import socket
import sys

server_addr = ('172.17.0.1',4200)
#server_addr = ('0.0.0.0',8080)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(server_addr)
msg = ''

while msg != '0':
    try:
        print >>sys.stderr, "Select from following"
        print >>sys.stderr, "[0] Exit"
        print >>sys.stderr, "[1] Quick response"
        print >>sys.stderr, "[2] Large file"
        print >>sys.stderr, "[3] UDP using Write instead of sendto (quick msg)"
        print >>sys.stderr, "[4] UDP using Write instead of sendto (large file)"
        msg = raw_input()
        sent = sock.send(msg)
        if msg == '1':
            print >>sys.stderr, "\n+[1] Quick response selected"
            system("python udp_clientQuickMsg.py")
        if msg == '2':
            print >>sys.stderr, "\n[2] Large file selected"
            system("python udp_clientBigfile.py")

        if msg == '3':
            print >>sys.stderr, "\n+[3] UDP using Write instead of sendto (quick msg) selected"
            system("python udp_clientQuickMsg.py")
        if msg == '4':
            print >>sys.stderr, "\n+[4] UDP using Write instead of sendto (large file) selected"
            system("python udp_clientBigfile.py")

    except Exception as e:
        print >>sys.stderr,e

print >>sys.stderr, "closing"
sock.close()
