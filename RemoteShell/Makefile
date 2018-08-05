FLAG = -std=c99 -Wall
SRC  = server.c ntutils.c
EXE  = server client

all: $(EXE)

server: server.o ntutils.o
	cc -o server server.o ntutils.o 

server.o: server.c ntutils.h
	cc -c $(FLAG) server.c

client: client.o ntutils.o
	cc -o client client.o ntutils.o 

client.o: client.c ntutils.h
	cc -c $(FLAG) client.c

ntutils.o: ntutils.c ntutils.h 
	cc -c $(FLAG) ntutils.c

clean:
	rm -f $(EXE) *.o
