#include<ulib.h>

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {
    char *ptr1 = mmap(NULL, 4096, PROT_WRITE|PROT_READ, 0);
    if((long)ptr1< 0)
    {
        printf("MMAP FAILED\n");
        return -1;
    }
    pmaps();
    char *ptr2 = mmap(NULL, 4096, PROT_READ, 0);
    if((long)ptr2< 0)
    {
        printf("MMAP FAILED\n");
        return -1;
    }
    pmaps();
    munmap(ptr1, 4096);
    munmap(ptr2, 4096);
    return 0;

}
