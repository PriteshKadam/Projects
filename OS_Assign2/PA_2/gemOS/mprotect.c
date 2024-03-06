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
#define USER_BIT  0x10

#define PAGE_SIZE_BYTES 4096


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

void merge_vmareas(struct exec_context* ctx) {

	struct vm_area* first = ctx->vm_area;
	if (first == NULL){
		return;
	}

	struct vm_area* second = first->vm_next;
	struct vm_area* temp = NULL;

	// Delete empty areas
	while (second) {
		if (second->vm_start == second->vm_end) {
			first->vm_next = second->vm_next;

			temp = second;
			second = second->vm_next;

			os_free(temp, sizeof(struct vm_area));
		}
		else {
			first = second;
			second = second->vm_next;
		}
	}

	// merge overlapping areas
	while (second) {
		if (first->vm_end >= second->vm_start && first->access_flags == second->access_flags) {
			first->vm_end = second->vm_end;
			first->vm_next = second->vm_next;

			temp = second;
			second = second->vm_next;

			os_free(temp, sizeof(struct vm_area));
		}
		else {
			first = second;
			second = second->vm_next;
		}
	}

	return;
}

void set_pagetable_protections(struct exec_context* ctx, u64 addr_start, u64 addr_end, int prot) {

	u48* cr3 = (u48*)osmap(ctx->pgd);
	u8 write_set = (prot & PROT_WRITE) ? 1 : 0;
	struct vm_area* vmarea;

	for (u48 page_addr = addr_start; page_addr < addr_end; page_addr += PAGE_SIZE_BYTES) {

		vmarea = ctx->vm_area->vm_next;
		while (vmarea != NULL) {
			if (vmarea->vm_start <= page_addr && vmarea->vm_end >= page_addr + PAGE_SIZE_BYTES) {
				break;
			}
			vmarea = vmarea->vm_next;
		}

		if (vmarea == NULL) {
			continue;
		}

		u48 pgd_offset = ((page_addr >> 39) & MASK9LSBITS);
		u48 pud_offset = ((page_addr >> 30) & MASK9LSBITS);
		u48 pmd_offset = ((page_addr >> 21) & MASK9LSBITS);
		u48 pte_offset = ((page_addr >> 12) & MASK9LSBITS);

		u48* pgd_t = cr3 + pgd_offset;
		if (*pgd_t & PRESENT_BIT == 0) {
			continue;
		}

		if (write_set == 1) {
			*pgd_t |= RW_BIT;
		}

		u48* pud = (u48*)osmap((*pgd_t) >> 12);
		u48* pud_t = pud + pud_offset;
		if (*pud_t & PRESENT_BIT == 0) {
			continue;
		}

		if (write_set == 1) {
			*pud_t |= RW_BIT;
		}

		u48* pmd = (u48*)osmap((*pud_t) >> 12);
		u48* pmd_t = pmd + pmd_offset;
		if (*pmd_t & PRESENT_BIT == 0) {
			continue;
		}

		if (write_set == 1) {
			*pmd_t |= RW_BIT;
		}

		u48* pte = (u48*)osmap((*pmd_t) >> 12);
		u48* pte_t = pte + pte_offset;
		if (*pmd_t & PRESENT_BIT == 0) {
			continue;
		}

		if (write_set == 1) {
			*pte_t |= RW_BIT;
		}
	}

	return;
}

u64 get_vmarea_count(struct exec_context* ctx) {
	struct vm_area* vmarea = ctx->vm_area;
	u64 count = 0;

	while (vmarea != NULL) {
		count++;
		vmarea = vmarea->vm_next;
	}

	return count;
}

long vm_area_mprotect(struct exec_context *current, u64 addr_start, int length, int prot)
{
	if (!current || !current->vm_area || length < 0 || !(prot == PROT_READ || prot == (PROT_READ | PROT_WRITE))) {
		return -1;
	}

	u64 mapped_region_size = ((length + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
	u64 addr_end = addr_start + mapped_region_size;

	struct vm_area* vmarea = current->vm_area->vm_next;

	while (vmarea != NULL) {
		if (vmarea->vm_start == addr_start && vmarea->vm_end <= addr_end) {
			vmarea->access_flags = prot;
		}else if (vmarea->vm_start == addr_start) {
			if (vmarea->access_flags != prot) {
				struct vm_area* new_vmarea = (struct vm_area*)os_alloc(sizeof(struct vm_area));
				new_vmarea->vm_start = addr_end;
				new_vmarea->vm_end = vmarea->vm_end;
				new_vmarea->access_flags = vmarea->access_flags;
				new_vmarea->vm_next = vmarea->vm_next;
				vmarea->vm_end = addr_end;
				vmarea->access_flags = prot;
				vmarea->vm_next = new_vmarea;
			}
		}
		else if (vmarea->vm_start < addr_start && vmarea->vm_end > addr_start) {
			if (vmarea->access_flags != prot) {
				struct vm_area* new_vmarea = (struct vm_area*)os_alloc(sizeof(struct vm_area));
				new_vmarea->vm_start = addr_start;
				new_vmarea->vm_end = vmarea->vm_end;
				new_vmarea->access_flags = prot;
				new_vmarea->vm_next = vmarea->vm_next;
				vmarea->vm_next = new_vmarea;

				if (vmarea->vm_end > addr_end) {
					struct vm_area* new_vmarea1 = (struct vm_area*)os_alloc(sizeof(struct vm_area));
					new_vmarea1->vm_start = addr_end;
					new_vmarea1->vm_end = vmarea->vm_end;
					new_vmarea1->vm_next = new_vmarea->vm_next;
					new_vmarea1->access_flags = vmarea->access_flags;
					new_vmarea->vm_next = new_vmarea1;
					new_vmarea->vm_end = addr_end;
				}
				vmarea->vm_end = addr_start;
			}
		}
		else if (vmarea->vm_start >= addr_start && vmarea->vm_end < addr_end) {
			vmarea->access_flags = prot;
		}
		else if (vmarea->vm_start < addr_end && vmarea->vm_end >= addr_end) {
			if (vmarea->access_flags != prot) {
				struct vm_area* new_vmarea = (struct vm_area*)os_alloc(sizeof(struct vm_area));
				new_vmarea->vm_start = addr_end;
				new_vmarea->vm_end = vmarea->vm_end;
				new_vmarea->access_flags = vmarea->access_flags;
				new_vmarea->vm_next = vmarea->vm_next;
				vmarea->access_flags = prot;
				vmarea->vm_next = new_vmarea;
				vmarea->vm_end = addr_end;
			}
		}

		vmarea = vmarea->vm_next;
	}

	merge_vmareas(current);
	printk("set_pagetable_protections.\n");
	stats->num_vm_area = get_vmarea_count(current);
	printk("count_vm_areas: %d.\n", stats->num_vm_area);
	set_pagetable_protections(current, addr_start, addr_end, prot);
	printk("returning.\n");
	return 0;
}

/**
 * Function will invoked whenever there is page fault for an address in the vmarea region
 * created using mmap
 */

u64 GetNewPte(int type)
{
    u64 new_pfn = (u64)os_pfn_alloc(type);
    u48 new_pte = (u48)osmap(new_pfn);
    new_pte = new_pte & (~MASK12LSBITS);
    new_pte = new_pte | PRESENT_BIT | USER_BIT | RW_BIT;
    return new_pte;
}

long vm_area_pagefault(struct exec_context *current, u64 addr, int error_code)
{
    u48 vaddr = addr;

    if (current == NULL || error_code == ERROR_CODE_WRITE_ON_READONLY) {
        //printk("vm_area_pagefault ERROR_CODE_WRITE_ON_READONLY \n");
        return -1;
    }

    // Verify addr 
    //printk("vm_area_pagefault Verify addr \n");
    struct vm_area* vmarea = current->vm_area;
    while (vmarea != NULL) {

        if (vaddr >= vmarea->vm_start && vaddr <= vmarea->vm_end){
            break;
        }
        vmarea = vmarea->vm_next;
    }
    
    if (vmarea == NULL){
        // No VMA exists
        //printk("vm_area_pagefault No VMA exists \n");
        return -1;
    }

    // Verify access flags
    if ((error_code == ERROR_CODE_WRITE) && (vmarea->access_flags == PROT_READ)){
        // Writing on read-only area or area having no write permission
        //printk("vm_area_pagefault Writing on read-only area \n");
        return -1;
    }
    // Names as per the 4 level page table diagram
    
    u48 pgd_offset = ((vaddr >> 39) & MASK9LSBITS); // First 9 bits
    u48 pud_offset = ((vaddr >> 30) & MASK9LSBITS); // second 9 bits
    u48 pmd_offset = ((vaddr >> 21) & MASK9LSBITS); // third 9 bits
    u48 pte_offset = ((vaddr >> 12) & MASK9LSBITS); // fourth 9 bits

    u48* cr3 = (u48*)osmap(current->pgd);
    u48* pgd_t = cr3 + pgd_offset;
    //printk("vm_area_pagefault pgd_t\n");
    if ((*pgd_t & 0x1) == 0){
        // Entry is not present. Need to allocate.
        //printk("vm_area_pagefault pgd_t allocation \n");
        *pgd_t = GetNewPte(OS_PT_REG);
    }

    u48* pud = (u48*)osmap((*pgd_t) >> 12);
    u48* pud_t = pud + pud_offset;
    //printk("vm_area_pagefault pud_t \n");
    if ((*pud_t & 0x1) == 0) {
        // Entry is not present. Need to allocate.
        //printk("vm_area_pagefault pud_t allocation \n");
        *pud_t = GetNewPte(OS_PT_REG);

    }

    u48* pmd = (u48*)osmap((*pud_t) >> 12);
    u48* pmd_t = pmd + pmd_offset;
    //printk("vm_area_pagefault pmd_t \n");
    if ((*pmd_t & 0x1) == 0) {
        // Entry is not present. Need to allocate.
        //printk("vm_area_pagefault pmd_t allocation \n");
        *pmd_t = GetNewPte(OS_PT_REG);
    }

	u48* pte = (u48*)osmap((*pmd_t) >> 12);
	u48* pte_t = pte + pte_offset;
    //printk("vm_area_pagefault pte_t \n");
    if ((*pte_t & 0x1) == 0){
        // Entry is not present. Need to allocate.
        //printk("vm_area_pagefault pte_t allocation \n");
        *pte_t = GetNewPte(USER_REG);
    }

    return 1;
}

