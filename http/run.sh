#!/usr/bin/bash

make
./proxy_server 4200 forbidden_sites&
gcc -o test src/client.c
./test
sleep 5;
kill $(pgrep proxy_server)
ps
