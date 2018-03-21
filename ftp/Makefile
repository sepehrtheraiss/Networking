BUG = -ggdb
EXTRA = -lm 
FLAG = -std=c99 -Wall $(EXTRA)
SRC = src/queue.c 
OBJ = obj/utils.o obj/server.o obj/client.o
EXE = bin/ftpserver bin/ftpclient 
MEMCHECK = valgrind --leak-check=full --track-origins=yes

all: $(EXE)

bin/ftpserver: obj/server.o  obj/utils.o
	cc -o bin/ftpserver obj/server.o obj/utils.o $(FLAG) 

obj/server.o: src/server.c include/utils.h
	cc -c src/server.c -o obj/server.o

bin/ftpclient: obj/client.o obj/utils.o 
	cc -o bin/ftpclient obj/client.o obj/utils.o $(FLAG) 

obj/client.o: src/client.c  include/utils.h 
	cc -c src/client.c -o obj/client.o 

obj/utils.o: src/utils.c include/utils.h
	cc -c src/utils.c -o obj/utils.o

clean:
	rm -f $(OBJ) $(EXE) $(EXE_T)
