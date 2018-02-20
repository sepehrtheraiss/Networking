FLAG = -std=c99 -Wall -ggdb
SRC = src/queue.c 
OBJ = obj/queue.o obj/queue_test.o
EXE = bin/q_test
TST = test/queue_test.c 
MEMCHECK = valgrind --leak-check=full --track-origins=yes


bin/q_test: obj/queue_test.o obj/queue.o 
	cc -o bin/q_test obj/queue_test.o obj/queue.o

obj/queue_test.o: test/queue_test.c include/queue.h
	cc -c test/queue_test.c -o obj/queue_test.o

obj/queue.o: src/queue.c include/queue.h
	cc -c $(FLAG) src/queue.c -o obj/queue.o

checkQ: bin/q_test
	$(MEMCHECK) ./bin/q_test
	
clean:
	rm -f $(OBJ) $(EXE) 
