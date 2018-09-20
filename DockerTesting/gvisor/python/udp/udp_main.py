#!/usr/bin/env python
from os import system
import socket
import sys

s_addr = ('0.0.0.0', 8080)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(s_addr)
sock.listen(1)
conn, addr = sock.accept()

data = 'blank'
while data != '0':
    print >>sys.stderr, "Select from following"
    print >>sys.stderr, "[0] Exit"
    print >>sys.stderr, "[1] Quick response"
    print >>sys.stderr, "[2] Large file"
    print >>sys.stderr, "[3] UDP using Write instead of sendto (quick msg)"
    print >>sys.stderr, "[4] UDP using Write instead of sendto (large file)"

    data = conn.recv(256)
    if data == '1':
        print >>sys.stderr, "\n\n+[1] Quick response selected"
        stat = system("python udp_quickMsgS.py")
        print >>sys.stderr, "status: %s" %stat

    if data  == '2':
        print >>sys.stderr, "\n\n+[2] Large file selected"
        system("python udp_bigfileS.py")

    if data == '3':
        print "[3] UDP using Write instead of sendto selected"
        system("./udpWrite")

print >>sys.stderr, "closing"
sock.close()
