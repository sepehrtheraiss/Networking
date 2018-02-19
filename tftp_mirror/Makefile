FLAG = -std=c99 -Wall -ggdb
SRC = src/queue.c 
OBJ = queue.o queue_test.o
EXE = q_test
TST = test/queue_test.c 
MEMCHECK = valgrind --leak-check=full --track-origins=yes


q_test: queue_test.o queue.o 
	cc -o q_test queue_test.o queue.o

queue_test.o: test/queue_test.c include/queue.h
	cc -c test/queue_test.c

queue.o: src/queue.c include/queue.h
	cc -c $(FLAG) src/queue.c

checkQ: q_test
	$(MEMCHECK) ./q_test
	
clean:
	rm -f $(OBJ) $(EXE) 
