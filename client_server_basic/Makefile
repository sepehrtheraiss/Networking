FLAG = -std=c99 -Wall -ggdb
SRC  = server.c client.c List.c test/test.c
OBJ  = src/server.o src/client.o src/List.o src/node.o src/test.o
EXE  = bin/server bin/client 
MEMCHECK = valgrind --leak-check=full --track-origins=yes

all: $(EXE)

bin/server: src/server.o src/List.o    
	cc -o bin/server src/server.o src/List.o

server.o: src/server.c header/List.h
	cc -c $(FLAG) src/server.c

bin/client: src/client.o src/List.o     
	cc -o bin/client src/client.o src/List.o

test: src/test.o src/List.o
	cc -o bin/test src/test.o src/List.o 

test.o: src/test.c header/List.h 
	cc -c src/test.c

testList: src/testList.o src/List.o
	cc -o bin/testList src/testList.o src/List.o

testList.o: src/testList.c header/List.h 
	cc -c src/testList.c

client.o: src/client.c header/List.h
	cc -c $(FLAG) src/client.c

List.o: src/List.c header/List.h
	cc -c $(FLAG) src/List.c

checkList: testList
	$(MEMCHECK) ./bin/testList "cat src/List.c"
	
clean:
	rm -f $(OBJ) $(EXE) bin/test
