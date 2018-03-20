#ifndef _UTILS_H_INCLUDE_
#define _UTILS_H_INCLUDE_

/* returns the file descriptor */
int bindIpp(char* ip,int port);
/* makes connection */
void conn(int sockfd,struct sockaddr_in* saddr);
/* if size > buff_size => chuncks = buff_size/size */
/* returns num bytes send */
off_t sendFile(int fd,char* buff,off_t size);
/* returns num bytes read */
off_t recvFile(int fd,char* buff);
#endif
