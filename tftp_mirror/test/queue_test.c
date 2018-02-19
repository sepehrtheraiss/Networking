#include "../include/queue.h"

int main()
{
    queue* q = q_init();
    printf("size:%i\n",q_size(q));
    q_insert(q,"hello");
    q_insert(q,"my name is");
    q_insert(q,"sepehr");
    printf("size:%i\n",q_size(q));
    printf("%s\n",q_front(q));
    q_remove(q);
    printf("%s\n",q_front(q));
    q_remove(q);
    printf("%s\n",q_front(q));
    q_remove(q);
    printf("size:%i\n",q_size(q));
    q_deinit(q);
return 0;
}
