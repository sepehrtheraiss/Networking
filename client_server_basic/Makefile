FLAG = -std=c99 -Wall
SRC  = server.c client.c stack.c node.c test/test.c
OBJ  = src/server.o src/client.o src/stack.o src/node.o src/test.o
EXE  = bin/server bin/client 

all: $(EXE)

bin/server: src/server.o src/stack.o src/node.o    
	cc -o bin/server src/server.o src/stack.o src/node.o 

server.o: src/server.c header/stack.h
	cc -c $(FLAG) src/server.c

bin/client: src/client.o src/stack.o src/node.o     
	cc -o bin/client src/client.o src/stack.o src/node.o 

test: src/test.o src/stack.o src/node.o
	cc -o bin/test src/test.o src/stack.o src/node.o 

test.o: src/test.c header/stack.h 
	cc -c src/test.c

client.o: src/client.c header/stack.h
	cc -c $(FLAG) src/client.c

stack.o: src/stack.c header/stack.h header/node.h
	cc -c $(FLAG) src/stack.c

node.o: src/node.c header/node.h
	cc -c $(FLAG) src/node.c

clean:
	rm -f $(OBJ) $(EXE) bin/test
