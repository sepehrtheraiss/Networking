#!/usr/bin/env python

import socket
import sys


TCP_IP = '0.0.0.0'
TCP_PORT = 8080
BUFFER_SIZE = 1024  # Normally 1024, but we want fast response

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)


print >>sys.stderr, 'tcp server'
conn, addr = s.accept()
print >>sys.stderr, 'Connection address:', addr

f = open("bigfile","r")
msg = f.read()
f.close()

data = conn.recv(BUFFER_SIZE)
print >>sys.stderr,"received data:", data
conn.send(msg)  # echo
conn.close()
