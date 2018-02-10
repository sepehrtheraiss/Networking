FLAG = -std=c99 -Wall -ggdb
SRC = server.c client.c stack.c node.c test/test.c
OBJ = src/server.o src/client.o src/stack.o src/node.o src/test.o src/testStack.o
EXE = bin/server bin/client bin/testStack
MEMCHECK = valgrind --leak-check=full --track-origins=yes

all: $(EXE)

bin/server: src/server.o 
	cc -o bin/server src/server.o $(FLAG)

src/server.o: src/server.c 
	cc -c $(FLAG) src/server.c -o $@

bin/client: src/client.o 
	cc -o bin/client src/client.o $(FLAG)

src/client.o: src/client.c 
	cc -c $(FLAG) src/client.c -o $@

test: src/test.o src/stack.o src/node.o
	cc -o bin/test src/test.o src/stack.o src/node.o  $(FLAG)

src/test.o: src/test.c header/stack.h 
	cc -c src/test.c $(FLAG)

bin/testStack: src/testStack.o src/stack.o src/node.o
	cc -o bin/testStack src/testStack.o src/stack.o src/node.o 

testStack.o: src/testStack.c header/stack.h 
	cc -c src/testStack.c

src/functions.o: src/functions.c header/header.h 
	cc -c $(FLAG) src/functions.c -o $@


src/stack.o: src/stack.c header/stack.h header/node.h
	cc -c $(FLAG) src/stack.c -o $@

src/node.o: src/node.c header/node.h
	cc -c $(FLAG) src/node.c -o $@

checkStack: testStack
	$(MEMCHECK) ./bin/testStack "cat src/stack.c"
	
clean:
	rm -f $(OBJ) $(EXE) bin/test
