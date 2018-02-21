FLAG = -std=c99 -Wall -ggdb
SRC = src/queue.c 
OBJ = obj/queue.o obj/queue_test.o obj/server.o obj/client.o obj/main.o obj/functions.o
EXE = bin/server bin/client 
EXE_T = bin/q_test bin/out
TST = test/queue_test.c test/main.c
MEMCHECK = valgrind --leak-check=full --track-origins=yes

all: $(EXE)

bin/server: obj/server.o  
	cc -o bin/server obj/server.o

obj/server.o: src/server.c  
	cc -c src/server.c -o obj/server.o

bin/client: obj/client.o
	cc -o bin/client obj/client.o

obj/client.o: src/client.c  
	cc -c src/client.c -o obj/client.o

bin/out: obj/main.o obj/functions.o
	cc -o bin/out obj/main.o obj/functions.o

obj/main.o: test/main.c include/header.h
	cc -c test/main.c -o obj/main.o

obj/functions.o: src/functions.c include/header.h
	cc -c src/functions.c -o obj/functions.o

bin/q_test: obj/queue_test.o obj/queue.o 
	cc -o bin/q_test obj/queue_test.o obj/queue.o

obj/queue_test.o: test/queue_test.c include/queue.h
	cc -c test/queue_test.c -o obj/queue_test.o

obj/queue.o: src/queue.c include/queue.h
	cc -c $(FLAG) src/queue.c -o obj/queue.o

checkQ: bin/q_test
	$(MEMCHECK) ./bin/q_test
	
clean:
	rm -f $(OBJ) $(EXE) $(EXE_T)
