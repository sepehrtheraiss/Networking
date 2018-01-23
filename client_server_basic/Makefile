FLAG = -std=c99 -Wall
SRC  = server.c client.c
OBJ  = server.o client.o
EXE  = server client

all: $(EXE)

server: server.o
	cc -o server server.o

server.o: server.c
	cc -c $(FLAG) server.c

client: client.o	 
	cc -o client client.o

client.o: client.c 
	cc -c $(FLAG) client.c
clean:
	rm -f $(OBJ) $(EXE)
