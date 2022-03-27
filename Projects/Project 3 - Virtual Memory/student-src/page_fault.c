/* MAKE SURE THESE NEXT LINES ARE UNCOMMENTED WHEN SUBMITTING*/

//#include "mmu.h"
//#include "pagesim.h"
//#include "swapops.h"
//#include "stats.h"

//THIS NEXT LINE MUST BE UNCOMMENTED TO RUN THIS SIMULATOR BUT MUST BE COMMENTED WHEN SUBMITTING
#include "../project3headertomakeeverythingwork.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * TODO 6: Get a new frame, then correctly update the page table and frame table
 * --------------------------------- PROBLEM 6 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Page fault handler.
 * 
 * When the CPU encounters an invalid address mapping in a page table, it invokes the 
 * OS via this handler. Your job is to put a mapping in place so that the translation 
 * can succeed.
 * 
 * @param addr virtual address in the page that needs to be mapped into main memory.
 * 
 * HINTS:
 *      - You will need to use the global variable current_process when
 *      altering the frame table entry.
 *      - Use swap_exists() and swap_read() to update the data in the 
 *      frame as it is mapped in.
 * ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t addr) {
   // TODO: Get a new frame, then correctly update the page table and frame table
    pte_t *pgtable = (pte_t *) MEMORY_LOC_OF_PFN(PTBR); //an array of page table entries (instead of PTBR should it
    // be current_process.ptbr?
    pte_t *page_table_entry = (pte_t *) pgtable + vaddr_vpn(addr); //a pointer to a specific page table entry
    pfn_t new_frame = free_frame();

    //swap_exists() takes in a page table entry (pte_t)
   if(swap_exists(page_table_entry)) {
       swap_read(page_table_entry, MEMORY_LOC_OF_PFN(new_frame));
   } else {
        memset(MEMORY_LOC_OF_PFN(new_frame), 0, PAGE_SIZE);
   }

   //update the processes' page table
   page_table_entry->valid = 0x1; //also not sure about this line
//   page_table_entry->dirty = 0x0; //not sure about this line
   page_table_entry->pfn = new_frame; //not sure about this one

   //update the frame table
   frame_table[new_frame].process = current_process;
   frame_table[new_frame].vpn = vaddr_vpn(addr);
}

#pragma GCC diagnostic pop
