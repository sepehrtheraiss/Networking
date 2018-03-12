FLAG = -std=c99 -Wall -ggdb
SRC = src/proxy_server.c
OBJ = obj/proxy_server.o
EXE = bin/proxy_server
MEMCHECK = valgrind --leak-check=full --track-origins=yes

all: $(EXE)

bin/proxy_server: obj/proxy_server.o
	cc -o bin/proxy_server obj/proxy_server.o $(FLAG)

obj/proxy_server.o: src/proxy_server.c
	cc -c $(FLAG) src/proxy_server.c -o obj/proxy_server.o

checkStack: test
	$(MEMCHECK) ./bin/test
	
clean:
	rm -f $(OBJ) $(EXE) bin/test
