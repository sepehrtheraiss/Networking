#include "../include/queue.h"

int main()
{
    queue* q = q_init();
    for(int i =0; i< 1000;i++)
    {
        q_insert(q,"hello");
    }
    q_deinit(q);
return 0;
}
