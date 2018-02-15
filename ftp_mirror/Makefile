EXTRA = -lpthread
FLAG = -std=c99 -Wall -ggdb -lm $(EXTRA)
SRC = server.c client.c
OBJ = src/server.o src/client.o src/functions.o
EXE = bin/server bin/client 
MEMCHECK = valgrind --leak-check=full --track-origins=yes

all: $(EXE)

bin/server: src/server.o src/functions.o 
	gcc -o bin/server src/server.o  src/functions.o $(FLAG)

src/server.o: src/server.c include/header.h 
	gcc -c $(FLAG) src/server.c -o $@

bin/client: src/client.o src/functions.o
	gcc -o bin/client src/client.o src/functions.o $(FLAG)

src/client.o: src/client.c include/header.h 
	gcc -c $(FLAG) src/client.c -o $@

src/functions.o: src/functions.c include/header.h 
	gcc -c $(FLAG) src/functions.c -o $@

checkStack: testStack
	$(MEMCHECK) ./bin/testStack "cat src/stack.c"
	
clean:
	rm -f $(OBJ) $(EXE) 
