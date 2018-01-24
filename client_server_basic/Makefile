FLAG = -std=c99 -Wall
SRC  = server.c client.c stack.c node.c
OBJ  = server.o client.o stack.o node.o 
EXE  = server client

all: $(EXE)

server: Src/server.o Src/stack.o Src/node.o    
	cc -o bin/server Src/server.o Src/stack.o Src/node.o 

server.o: Src/server.c Header/stack.h
	cc -c $(FLAG) Src/server.c

stack.o: Src/stack.c Header/stack.h Header/node.h
	cc -c $(FLAG) Src/stack.c

node.o: Src/node.c Header/node.h
	cc -c $(FLAG) Src/node.c

client: Src/client.o	 
	cc -o bin/client Src/client.o

client.o: Src/client.c 
	cc -c $(FLAG) Src/client.c

clean:
	rm -f $(OBJ) $(EXE)
