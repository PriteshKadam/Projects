#include<ulib.h>

void task1_testcase1()
{
    printf("testcase1 \n");
    int a = 10;
    pmaps();
    return;
}

void task1_testcase2()
{
    printf("testcase2 \n");
    int a = 10;
    pmaps();
    char* ptr1 = mmap(NULL, 4096, PROT_WRITE, 0);
    if ((long)ptr1 < 0)
    {
        printf("MMAP FAILED\n");
        return;
    }
    pmaps();
    munmap(ptr1, 4096);
    return;
}

void task1_testcase3() {
    printf("testcase3 \n");
    char* ptr1 = mmap(NULL, 4096, PROT_WRITE | PROT_READ, 0);
    if ((long)ptr1 < 0)
    {
        printf("MMAP FAILED\n");
        return;
    }
    pmaps();
    char* ptr2 = mmap(NULL, 4096, PROT_READ, 0);
    if ((long)ptr2 < 0)
    {
        printf("MMAP FAILED\n");
        return;
    }
    pmaps();
    munmap(ptr1, 4096);
    munmap(ptr2, 4096);
    return;

}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  
  printf("Hello World\n");
  task1_testcase1();
  task1_testcase2();
  task1_testcase3();

  return 0;
}

