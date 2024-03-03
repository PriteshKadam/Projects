#include <types.h>
#include <mmap.h>
#include <fork.h>
#include <v2p.h>
#include <page.h>

/*
 * You may define macros and other helper functions here
 * You must not declare and use any static/global variables
 * */

#define ERROR_CODE_READ 4
#define ERROR_CODE_WRITE 6
#define ERROR_CODE_WRITE_ON_READONLY 7

#define MASK9LSBITS 0x1FF
#define MASK12LSBITS 0xFFF

#define PRESENT_BIT 0x1
#define RW_BIT  0x8



// For readability : 48 bit address
#define u48 u64 
/*
 * Your Task-1 implementation
 */

void getAccessFlagText(u32 access_flags, char* accessText)
{
    // Get permissions
    if (access_flags & PROT_READ){
        accessText[0] = 'R';
    }else {
        accessText[0] = '-';
    }
    if (access_flags & PROT_WRITE){
        accessText[1] = 'W';
    }else {
        accessText[1] = '-';
    }
    if (access_flags & PROT_EXEC){
        accessText[2] = 'X';
    }else {
        accessText[2] = '-';
    }

    return;
}

int print_pmaps(struct exec_context *ctx){
    if (!ctx) {
        return -1;
    }
    char permissions[3];

    // Code segment 
    getAccessFlagText(ctx->mms[MM_SEG_CODE].access_flags, permissions);
    printk("%x  %x %c%c%c CODE \n", ctx->mms[MM_SEG_CODE].start, ctx->mms[MM_SEG_CODE].end, permissions[0], permissions[1], permissions[2]);

    // Data segment
    getAccessFlagText(ctx->mms[MM_SEG_DATA].access_flags, permissions);
    printk("%x  %x %c%c%c DATA \n", ctx->mms[MM_SEG_DATA].start, ctx->mms[MM_SEG_DATA].end, permissions[0], permissions[1], permissions[2]);

    // MMAP
    struct vm_area* vmarea = ctx->vm_area;
    while (vmarea != NULL){
        getAccessFlagText(vmarea->access_flags, permissions);
        printk("%x  %x %c%c%c MMAP \n", vmarea->vm_start, vmarea->vm_end, permissions[0], permissions[1], permissions[2]);
        vmarea = vmarea->vm_next;
    }

    // Stack segment
    getAccessFlagText(ctx->mms[MM_SEG_STACK].access_flags, permissions);
    printk("%x  %x %c%c%c STACK \n", ctx->mms[MM_SEG_STACK].start, ctx->mms[MM_SEG_STACK].end, permissions[0], permissions[1], permissions[2]);

    return 0;
}


/**
 * mprotect System call Implementation.
 */

long vm_area_mprotect(struct exec_context *current, u64 addr, int length, int prot)
{
        return -EINVAL;
}

/**
 * Function will invoked whenever there is page fault for an address in the vm area region
 * created using mmap
 */

long vm_area_pagefault(struct exec_context *current, u64 addr, int error_code)
{
    u48 vaddr = addr;

    if (current == NULL || error_code == ERROR_CODE_WRITE_ON_READONLY) {
        return -1;
    }

    char permissions[3];

    // Verify addr 
    struct vm_area* vmarea = current->vm_area;
    while (vmarea != NULL) {

        if (vaddr >= vmarea->vm_start && vaddr <= vmarea->vm_end){
            break;
        }
        vmarea = vmarea->vm_next;
    }

    if (vmarea == NULL){
        // No VMA exists
        return -1;
    }

    // Verify access flags
    if ((error_code == ERROR_CODE_WRITE) && !(vmarea->access_flags & PROT_READ)){
        // Writing on read-only area or area having no write permission
        return -1;
    }
    // Names as per the 4 level page table diagram
    
    u48 pgd_offset = ((vaddr >> 39) & MASK9LSBITS); // First 9 bits
    u48 pud_offset = ((vaddr >> 30) & MASK9LSBITS); // second 9 bits
    u48 pmd_offset = ((vaddr >> 21) & MASK9LSBITS); // third 9 bits
    u48 pte_offset = ((vaddr >> 12) & MASK9LSBITS); // fourth 9 bits

    u48* cr3 = (u48*)osmap(current->pgd);
    u48* pgd_t = cr3 + pgd_offset;
    if ((*pgd_t & 0x1) == 0){
        // Entry is not present. Need to allocate.
        u64 new_pfn = (u64)os_pfn_alloc(OS_PT_REG);
        u48 new_pte = (u48)osmap(new_pfn);
        new_pte = new_pte & (~MASK12LSBITS);
        new_pte  = new_pte | PRESENT_BIT | RW_BIT;
        *pgd_t = new_pte;
    }

    u48* pud = (u48*)osmap((*pgd_t) >> 12);
    u48* pud_t = pud + pud_offset;
    if ((*pud_t & 0x1) == 0) {
        // Entry is not present. Need to allocate.
        u64 new_pfn = (u64)os_pfn_alloc(OS_PT_REG);
        u48 new_pte = (u48)osmap(new_pfn);
        new_pte = new_pte & (~MASK12LSBITS);
        new_pte = new_pte | PRESENT_BIT | RW_BIT;
        *pud_t = new_pte;

    }

    u48* pmd = (u48*)osmap((*pud_t) >> 12);
    u48* pmd_t = pmd + pmd_offset;
    if ((*pmd_t & 0x1) == 0) {
        // Entry is not present. Need to allocate.
        u64 new_pfn = (u64)os_pfn_alloc(OS_PT_REG);
        u48 new_pte = (u48)osmap(new_pfn);
        new_pte = new_pte & (~MASK12LSBITS);
        new_pte = new_pte | PRESENT_BIT | RW_BIT;
        *pmd_t = new_pte;
    }

    u64* pte = (u64*)osmap((*pmd_t) >> 12);
    u64* pte_t = pte + pte_offset;
    if ((*pte_t & 0x1) == 0){
        // Entry is not present. Need to allocate.
        u64 new_pfn = (u64)os_pfn_alloc(USER_REG);
        u48 new_pte = (u48)osmap(new_pfn);
        new_pte = new_pte & (~MASK12LSBITS);
        new_pte = new_pte | PRESENT_BIT | RW_BIT;
        *pte_t = new_pte;
    }

    return 1;
}

