#include "../include/utils.h"

int main(int argc, char** argv) {
	if(argc < 3)
	{
		fprintf(stderr, "usage: %s server-ip server-port\n",argv[0]);
		exit(1);
	}

	if(inet_pton(AF_INET,argv[1],&IP)<1){
      perror("ERROR pasring IP address");
      exit(EXIT_FAILURE);
   	} 
   portno = atoi(argv[2]);
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(EXIT_FAILURE);
   }
	struct sockaddr_in serv_addr;
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = IP;//htonl(INADDR_LOOPBACK);
   serv_addr.sin_port = htons(portno);
   
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(EXIT_FAILURE);
   }
   char buffer[MAX_CMD_LEN];
   memset(buffer,0,MAX_CMD_LEN);
   int n;
   int q=0;
   char* token[3];
   while(q!=1)
   {
   		printf("ftp> ");
   		n = read(STDIN_FILENO,buffer,MAX_CMD_LEN); // returns counted characters, exclude \0
   		splitString(" ",buffer,token);
   		if(sendCMD(sockfd,token[0],token[1],&serv_addr)==221)
   		{
   			q = 1;
   		}
   }


	return 0;
}