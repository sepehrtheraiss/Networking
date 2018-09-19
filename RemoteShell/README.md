# Remote Shell

**This program currently handles read only commands**

#### Protocol:  
  * A header consist of packet ID, state and size of payload seperated by ':' 
  * After header is created a header size pakcet consist of size of header will be preppend to header.(header size packet length will always be size of BUFFSIZE, filled with \0)
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
  * Optimize header size packet to be max of header size. i.e less than BUFFSIZE.
  * Handle signals such as Control-C for htop to resume.
