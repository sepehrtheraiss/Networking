#include "../include/utils.h"

int main()
{
    struct sockaddr_in serv_addr;
    bindIpp("127.0.0.1",4200, &serv_addr);
    return 0;
}
