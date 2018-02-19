EXTRA = -lm -lpthread
FLAG = -std=c99 -Wall -ggdb $(EXTRA)
SRC = server.c client.c
OBJ = server.o client.o functions.o
EXE = bin/server bin/client 
MEMCHECK = valgrind --leak-check=full --track-origins=yes

all: $(EXE)

bin/server: src/server.o functions.o 
	gcc -o bin/server src/server.o  functions.o $(FLAG)

server.o: src/server.c include/header.h 
	gcc -c $(FLAG) src/server.c 

bin/client: src/client.o src/functions.o
	gcc -o bin/client src/client.o functions.o $(FLAG)

client.o: src/client.c include/header.h 
	gcc -c $(FLAG) src/client.c 

functions.o: src/functions.c include/header.h 
	gcc -c $(FLAG) src/functions.c 

checkStack: testStack
	$(MEMCHECK) ./bin/testStack "cat src/stack.c"
	
clean:
	rm -f $(OBJ) $(EXE) 
