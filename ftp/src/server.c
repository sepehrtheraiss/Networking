#include "../include/utils.h"
int main(int argc, char** argv) {
	if(argc < 2)
	{
		fprintf(stderr, "usage: %s listen-port\n",argv[0]);
		exit(1);
	}
	struct sockaddr_in serv_addr,cli_addr;
	int sockfd = bindIpp("0.0.0.0",atoi(argv[2]),&serv_addr,0);

	listen(sockfd,5);
	while(1)
	{
		int clisockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
		if(fork()==0)
		{
			while(recvCMD(int sockfd,struct sockaddr_in* cli_addr) != 221);
		}			
	}
	return 0;
}