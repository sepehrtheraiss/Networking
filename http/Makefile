FLAG = -std=c99 -Wall -ggdb
SRC = src/proxy_server.c src/utils.c
OBJ = obj/proxy_server.o obj/utils.o
EXE = bin/proxy_server
MEMCHECK = valgrind --leak-check=full --track-origins=yes

all: $(EXE)

bin/proxy_server: obj/proxy_server.o obj/utils.o
	gcc -o bin/proxy_server obj/proxy_server.o obj/utils.o $(FLAG)

obj/proxy_server.o: src/proxy_server.c include/header.h
	gcc -c src/proxy_server.c -o obj/proxy_server.o $(FLAG)

obj/utils.o: src/utils.c include/header.h
	gcc -c src/utils.c -o obj/utils.o $(FLAG)

checkStack: test
	$(MEMCHECK) ./bin/test
	
clean:
	rm -f $(OBJ) $(EXE) bin/test
