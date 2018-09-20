import socket

IP = '127.0.0.1'
port = 8080
msg = 'tcp message#'

s = socket.socket()
host = socket.gethostname()
s.connect((IP,port))
for i in range(5):
    s.send(msg+str(i)+'\n')

s.close()
