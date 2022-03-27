/* MAKE SURE THESE NEXT LINES ARE UNCOMMENTED WHEN SUBMITTING*/

//#include "mmu.h"
//#include "pagesim.h"
//#include "va_splitting.h"
//#include "swapops.h"
//#include "stats.h"

//THIS NEXT LINE MUST BE UNCOMMENTED TO RUN THIS SIMULATOR BUT MUST BE COMMENTED WHEN SUBMITTING
#include "../project3headertomakeeverythingwork.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* The frame table pointer. You will set this up in system_init. */
fte_t *frame_table;

/**
 * TODO 2: initialize system
 * --------------------------------- PROBLEM 2 --------------------------------------
 * Checkout PDF sections 4 for this problem
 * 
 * In this problem, you will initialize the frame_table pointer. The frame table will
 * be located at physical address 0 in our simulated memory. You should zero out the 
 * entries in the frame table, in case for any reason physical memory is not clean.
 * 
 * HINTS:
 *      - mem: Simulated physical memory already allocated for you.
 *      - PAGE_SIZE: The size of one page
 * ----------------------------------------------------------------------------------
 */
void system_init(void) {
    frame_table = (fte_t *) mem;
    memset(frame_table, 0, PAGE_SIZE); //just making sure the frame_table entries are initialized to zero
    frame_table->protected = 0x1; //Set frame table to protected (we don't want to evict this!)
}

/**
 * TODO 5: translate virtual address to physical, then perform the specified operation
 * --------------------------------- PROBLEM 5 --------------------------------------
 * Checkout PDF section 6 for this problem
 * 
 * Takes an input virtual address and performs a memory operation.
 * 
 * @param addr virtual address to be translated
 * @param rw   'r' if the access is a read, 'w' if a write
 * @param data If the access is a write, one byte of data to write to our memory.
 *             Otherwise NULL for read accesses.
 * 
 * HINTS:
 *      - Remember that not all the entries in the process's page table are mapped in.
 *      Check what in the pte_t struct signals that the entry is mapped in memory.
 * ----------------------------------------------------------------------------------
 */
uint8_t mem_access(vaddr_t addr, char rw, uint8_t data) {
    pte_t *pgtable = (pte_t *) MEMORY_LOC_OF_PFN(PTBR);

    /* Either read or write the data to the physical address
       depending on 'rw' */
    if (rw == 'r') {

    } else {

    }

    return 0;
}