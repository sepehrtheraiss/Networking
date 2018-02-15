#include "../include/header.h"
int main(int argc, char** argv) {
    /*usage: server-info.txt num-connections filename*/
    if (argc < 4) {
        fprintf(stderr,"usage: %s server-info.txt num-connections filename\n", argv[0]);
        exit(0);
    }
    server servers[10];
    up = 0;
    int nc = atoi(argv[2]); // pass in server nums by the user
    strcpy(filename,argv[3]);
    FILE* s_file = fopen(argv[1],"r");
    if(s_file == NULL){perror("error fopen on argv[1]");exit(1);}
    int num_servs = server_info(s_file,servers); // number of servers in the server-info
    fclose(s_file);
    // number of connections can't be more than the number of servers in the server-info.txt
    if(nc > num_servs)
    {
        nc = num_servs;
    }
    // gets the file size from the first responsive server
    if((fileSize = getFileSize(servers,filename,num_servs,nc)) < 1)
    {
        perror("file either does not exist or all the servers are down");
        exit(1);
    }
    //printf("File size: %li\n",fileSize);
    if(fileSize > 1073741824) // 1 GIG
    {
        perror("file too big");
        exit(1);
    }
    for(int i =0;i < num_servs;i++)
    {
        servers[i].id = -1;
    }
    // checks which servers are up then assigns id to each one
    // need to find max number of servers up
    for(int i=0; i < num_servs && up < nc;i++)
    {
        if(isUp(&servers[i]) == 1)
        {
            servers[i].str_arr = malloc(sizeof(char*)*30); // 1 GIG
            servers[i].id = up++;
        }
    }
    SUP = up ;// serves up this cannot be modified
    pthread_t thread[up];
    for(int i =0; i < num_servs;i++)
    {
        if(servers[i].id != -1)
        {
            pthread_create(&thread[i],NULL,(void *)initThread,(server*)&servers[i]); 
        }
    }
    while(up != 0);// busy waiting until all servs have done their job
    char newFile[128];
    strcpy(newFile,filename);
    strcat(newFile,"01");
    creat(newFile, S_IRUSR+S_IWUSR);
    FILE* nf = fopen(newFile,"a");
    for(int i =0; i < num_servs;i++)
    {
        if(servers[i].id != -1)
        {
            for(int j=0;servers[i].str_arr[j]!=NULL && j < 10;j++)
            {
    //            printf("%s",servers[i].str_arr[j]);
                fprintf(nf,"%s",servers[i].str_arr[j]);
                free(servers[i].str_arr[j]);
            }
            free(servers[i].str_arr);
        }
    }
    fclose(nf);
    return 0;
}
