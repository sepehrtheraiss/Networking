# Remote Shell

**This program currently only handles read only commands**

Protocol:
   SendMsg formats a header size of packet in hex format.  
   Max format header size is 32Bytes + 3 bytes 0x and null terminator.  
   Unoccupied space in format header is padded with zeros prepended to 0x.  
   Max packet size is 32Bytes.  
   ReadMsg finds the first occurance of 'x' then examines the rest of header format.  

Files:

    Makefile
    README

    include:
    ntutils.h

    src:
    client.c
    ntutils.c
    server.c

