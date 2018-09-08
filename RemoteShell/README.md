# Remote Shell

**This program currently handles read only commands**

#### Protocol:  
  * A header consist of packet ID, state and size of payload seperated by ':' 
  * After header is created a header size pakcet consist of size of header will be preppend to header.(header size length will always be size of BUFFSIZE, filled with \0)
  * After header the payload is send
  * Example: 5 0:0:2 ls
#### Files:

    Makefile
    README

    include:
    ntutils.h

    src:
    client.c
    ntutils.c
    server.c

TODO: 
    * optimize header size packet to be max of header size. i.e less than BUFFSIZE
    * handle signals such has Control-C for htop to resume 
