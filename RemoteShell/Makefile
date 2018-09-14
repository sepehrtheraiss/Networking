FLAG = -std=gnu99 -Wall
SRC  = server.c ntutils.c
EXE  = bin/server bin/client

all: $(EXE)

bin/server: obj/server.o obj/ntutils.o
	cc -o bin/server obj/server.o obj/ntutils.o 

obj/server.o: src/server.c include/ntutils.h
	cc -c $(FLAG) src/server.c -o obj/server.o

bin/client: obj/client.o obj/ntutils.o
	cc -o bin/client obj/client.o obj/ntutils.o 

obj/client.o: src/client.c include/ntutils.h
	cc -c $(FLAG) src/client.c -o obj/client.o

obj/ntutils.o: src/ntutils.c include/ntutils.h 
	cc -c $(FLAG) src/ntutils.c -o obj/ntutils.o

clean:
	rm -f $(EXE) obj/*.o
