/* MAKE SURE THESE NEXT LINES ARE UNCOMMENTED WHEN SUBMITTING*/

//#include "types.h"
//#include "pagesim.h"
//#include "mmu.h"
//#include "swapops.h"
//#include "stats.h"
//#include "util.h"

//THIS NEXT LINE MUST BE UNCOMMENTED TO RUN THIS SIMULATOR BUT MUST BE COMMENTED WHEN SUBMITTING
#include "../project3headertomakeeverythingwork.h"

pfn_t select_victim_frame(void);

pfn_t last_evicted = 0; //basically the head to our circular queues for FIFO and CLOCK algo

/**
 * TODO 7: evict any mapped pages.
 * --------------------------------- PROBLEM 7 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Make a free frame for the system to use. You call the select_victim_frame() method
 * to identify an "available" frame in the system (already given). You will need to 
 * check to see if this frame is already mapped in, and if it is, you need to evict it.
 * 
 * @return victim_pfn: a physical frame number to a free frame be used by other functions.
 * 
 * HINTS:
 *      - When evicting pages, remember what you checked for to trigger page faults 
 *      in mem_access
 *      - If the page table entry has been written to before, you will need to use
 *      swap_write() to save the contents to the swap queue.
 * ----------------------------------------------------------------------------------
 */
pfn_t free_frame(void) {
    pfn_t victim_pfn = select_victim_frame();

    // TODO: evict any mapped pages.
    if (frame_table[victim_pfn].mapped) {
        pte_t *page_table_entry = (pte_t *) MEMORY_LOC_OF_PFN(frame_table[victim_pfn].process->saved_ptbr);
//        page_table_entry->valid = 0x0;

        //has the page been written to since being created or loaded from the swap space?
        if (page_table_entry->dirty) {
            //yes, the page has been written to. We need to put this page in the swap space.

            swap_write(page_table_entry, MEMORY_LOC_OF_PFN(victim_pfn));
            page_table_entry->dirty = 0x0; //page is no longer dirty since we are writing it to the swap space

            stats.writebacks++;
        } //else the page was not written to so no data is lost if we overwrite it

        page_table_entry->valid = 0x0;

//        frame_table[victim_pfn].mapped = 0x0;
        frame_table[victim_pfn].process = NULL;
        frame_table[victim_pfn].protected = 0;
        frame_table[victim_pfn].mapped = 0;
        frame_table[victim_pfn].referenced = 0;
        frame_table[victim_pfn].vpn = 0;

        memset(page_table_entry, 0, sizeof(pte_t));
    }

    return victim_pfn;
}



/**
 * TODO 9: Selection of the Victim Frame
 * --------------------------------- PROBLEM 9 --------------------------------------
 * Checkout PDF section 7, 9, and 11 for this problem
 * 
 * Finds a free physical frame. If none are available, uses either a
 * randomized, FCFS, or clocksweep algorithm to find a used frame for
 * eviction.
 * 
 * @return The physical frame number of a victim frame.
 * 
 * HINTS: 
 *      - Use the global variables MEM_SIZE and PAGE_SIZE to calculate
 *      the number of entries in the frame table.
 *      - Use the global last_evicted to keep track of the pointer into the frame table
 * ----------------------------------------------------------------------------------
 */
pfn_t select_victim_frame() {
    /* See if there are any free frames first */
    size_t num_entries = MEM_SIZE / PAGE_SIZE;
    for (size_t i = 0; i < num_entries; i++) {
        if (!frame_table[i].protected && !frame_table[i].mapped) {
            return i;
        }
    }

    // RANDOM implemented for you.
    if (replacement == RANDOM) {
        /* Play Russian Roulette to decide which frame to evict */
        pfn_t last_unprotected = NUM_FRAMES;
        for (pfn_t i = 0; i < num_entries; i++) {
            if (!frame_table[i].protected) {
                last_unprotected = i;
                if (prng_rand() % 2) {
                    return i;
                }
            }
        }
        /* If no victim found yet take the last unprotected frame
           seen */
        if (last_unprotected < NUM_FRAMES) {
            return last_unprotected;
        }


    } else if (replacement == FIFO) {
        // TODO 9.2: Implement a FIFO algorithm here
        if (frame_table[last_evicted].protected == 0) {
            return last_evicted;
        }

        if (frame_table[last_evicted + 1].process != NULL) {
            last_evicted++;
        } else {
            last_evicted = 0;
        }
    } else if (replacement == CLOCKSWEEP) {
        // TODO 9.1: Implement a clocksweep page replacement algorithm here
        for (size_t i = 0; i < num_entries; i++) { //start at one since the frame table is technically at 1
            if (frame_table[last_evicted].protected == 0 && frame_table[last_evicted].referenced == 0) {
                return last_evicted;
            } else if (frame_table[last_evicted].protected == 0 && frame_table[last_evicted].referenced == 1) {
                frame_table[last_evicted].referenced = 0x0; //reset reference bit
            }


            if (frame_table[last_evicted + 1].process != NULL) {
                last_evicted++;
            } else {
                last_evicted = 0;
            }
        }
    }

    /* If every frame is protected, give up. This should never happen
       on the traces we provide you. */
    panic("System ran out of memory\n");
    exit(1);
}
