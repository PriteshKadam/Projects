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

int task2_tc1()
{
    int pages = 4096;

    // vm_area will be created without physical pages.
    //printf("task2_tc1\n");
    char* lazy_alloc = mmap(NULL, pages * 50, PROT_READ | PROT_WRITE, 0);
    if ((long)lazy_alloc < 0)
    {
        // Testcase failed.
        printf("mmap failed \n");
        return 1;
    }
    //printf("lazy_alloc Done\n");
    // All accesses should result in page fault.
    for (int i = 0; i < 50; i++)
    {
        lazy_alloc[(pages * i)] = 'X';
    }
    //printf("All accessed Done\n");
    // Number of MMAP_Page_Faults should be 50 & 
    // Number of vm_area should 1
    pmap(0);
    //printf("Reading the value from physical page\n");
    for (int i = 0; i < 50; i++)
    {
        // Reading the value from physical page. It should be same as written
        if (lazy_alloc[(pages * i)] != 'X')
        {
            // Testcase Failed;
            printf("Test case failed \n");
            return 0;
        }
    }
    //printf("Reading the value from physical page done\n");
    // Number of MMAP_Page_Faults should be 50 & 
    // Number of vm_area should 1
    pmap(0);

    printf("Reached end of the program\n");

    return 0;
}


int task2_tc2()
{
    int pages = 4096;

    char* mm1 = mmap(NULL, pages * 6, PROT_READ | PROT_WRITE, 0);
    if ((long)mm1 < 0)
    {
        // Testcase failed.
        printf("mmap failed \n");
        return 1;
    }

    char* readonly_address = (char*)((unsigned long)mm1 + pages * 3);
    //should result in page fault
    readonly_address[0] = 'X';

    // vm_area count should be 1 and Page fault should be 1
    pmap(0);

    if (readonly_address[0] != 'X')
    {
        // Testcase failed
        printf("Test case failed \n");
        return 0;
    }


    // Should change access rights of the third page, existing vm_Area should be splitted up. 
    // A new vm area with access rights with PROT_READ will be created.
    int result = mprotect((void*)readonly_address, pages, PROT_READ);
    if (result < 0)
    {
        // Testcase failed
        printf("Test case failed \n");
        return 0;
    }
    printf("mprotect end\n");
    // vm_area count should be 3.
    pmap(1);

    printf("Reached end of the program\n");

    // Access violation, Writing to a read only address. Process should be terminated while executing the below line

    printf("Returning\n");
    return 0;
}


int task2_tc3()
{
    int pages = 4096;
    char* mm1 = mmap(NULL, pages * 6, PROT_READ, 0);
    if ((long)mm1 < 0)
    {
        // Testcase failed.
        printf("mmap failed \n");
        return 1;
    }

    //Read all the pages
    for (int i = 0; i < 6; i++) {
        char temp;
        char* page_read = mm1 + (i * pages);
        temp = page_read[0];
    }
    //Page faults should be 6
    pmap(0);

    //changing the protection should give access to write
    int result = mprotect((void*)mm1, pages * 6, PROT_READ | PROT_WRITE);
    if (result < 0)
    {
        printf("Test case failed \n");
        return 0;
    }

    for (int i = 0; i < 6; i++) {
        char* page_write = mm1 + (i * pages);
        page_write[0] = 'A';
    }
    //Page faults should be 6
    pmap(0);

    printf("Reached end of the program\n");

    return 0;
}

int task3_tc1()
{

    int pages = 4096;

    char* mm1 = mmap(NULL, pages * 6, PROT_READ | PROT_WRITE, 0);
    if ((long)mm1 < 0)
    {
        printf("mmap failed \n");
        return 1;
    }
    // vm_area count should be 1.
    pmap(1);

    int result = mprotect((void*)mm1, pages, PROT_READ);

    if (result < 0)
    {
        // Testcase failed
        printf("Test case failed \n");
        return 0;
    }

    // vm_area count should be 2.
    pmap(1);

    return 0;
}

int task3_tc2()
{

    int pages = 4096;

    char* mm1 = mmap(NULL, pages * 6, PROT_READ | PROT_WRITE, 0);
    if ((long)mm1 < 0)
    {
        printf("mmap failed \n");
        return 1;
    }
    // vm_area count should be 1.
    pmap(1);

    unsigned long readonly_address = (unsigned long)mm1 + pages * 3;

    int result = mprotect((void*)readonly_address, pages, PROT_READ);

    if (result < 0)
    {
        // Testcase failed
        printf("Test case failed \n");
        return 0;
    }

    // vm_area count should be 3.
    pmap(1);

    return 0;
}


int task3_tc3()
{
    int pages = 4096;

    char* mm1 = mmap(NULL, pages * 2, PROT_READ | PROT_WRITE, 0);
    if ((long)mm1 < 0)
    {
        printf("mmap failed \n");
        return 1;
    }
    pmap(1);

    char* mm2 = mmap(NULL, pages * 3, PROT_READ, 0);
    if ((long)mm2 < 0)
    {
        printf("mmap failed \n");
        return 1;
    }
    pmap(1);


    int val2 = mprotect((void*)mm1, pages * 5, PROT_READ);
    if (val2 < 0)
    {
        printf("Test case failed \n");
        return 1;
    }
    pmap(1);

    return 0;
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  
  //printf("Hello World\n");
  /*
  task1_testcase1();
  task1_testcase2();
  task1_testcase3();
  */
    //task2_tc1();
    task2_tc2();
    //task2_tc3();

    //task3_tc1();
   //task3_tc2();
    task3_tc3();
  return 0;
}

