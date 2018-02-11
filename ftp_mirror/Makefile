FLAG = -std=c99 -Wall -ggdb
SRC = server.c client.c stack.c node.c 
OBJ = src/server.o src/client.o src/stack.o src/node.o src/functions.o
EXE = bin/server bin/client 
MEMCHECK = valgrind --leak-check=full --track-origins=yes

all: $(EXE)

bin/server: src/server.o 
	cc -o bin/server src/server.o $(FLAG)

src/server.o: src/server.c 
	cc -c $(FLAG) src/server.c -o $@

bin/client: src/client.o src/functions.o
	cc -o bin/client src/client.o src/functions.o $(FLAG)

src/client.o: src/client.c include/header.h 
	cc -c $(FLAG) src/client.c -o $@

src/functions.o: src/functions.c include/header.h 
	cc -c $(FLAG) src/functions.c -o $@

src/stack.o: src/stack.c include/stack.h include/node.h
	cc -c $(FLAG) src/stack.c -o $@

src/node.o: src/node.c include/node.h
	cc -c $(FLAG) src/node.c -o $@

checkStack: testStack
	$(MEMCHECK) ./bin/testStack "cat src/stack.c"
	
clean:
	rm -f $(OBJ) $(EXE) bin/test
