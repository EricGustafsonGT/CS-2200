//#include "proc.h"
//#include "mmu.h"
//#include "pagesim.h"
//#include "va_splitting.h"
//#include "swapops.h"
//#include "stats.h"

//THIS NEXT LINE MUST BE UNCOMMENTED TO RUN THIS SIMULATOR BUT MUST BE COMMENTED WHEN SUBMITTING
#include "../project3headertomakeeverythingwork.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 3 --------------------------------------
 * Checkout PDF section 4 for this problem
 * 
 * This function gets called every time a new process is created.
 * You will need to allocate a frame for the process's page table using the
 * free_frame function. Then, you will need update both the frame table and
 * the process's PCB. 
 * 
 * @param proc pointer to process that is being initialized 
 * 
 * HINTS:
 *      - pcb_t: struct defined in pagesim.h that is a process's PCB.
 *      - You are not guaranteed that the memory returned by the free frame allocator
 *      is empty - an existing frame could have been evicted for our new page table.
 * ----------------------------------------------------------------------------------
 */   //(pte_t *)(mem + (found_ptbr * PAGE_SIZE)); 290
void proc_init(pcb_t *proc) {
    //Step 1.) Get a free frame to allocate
    pfn_t frame_number_to_be_allocated = free_frame();

    //Each frame contains PAGE_SIZE bytes of data, therefore to access the start of the i-th frame in memory, you
    //can use mem + (i * PAGE_SIZE). Here, the frame number we just got is the "i" in the formula. The defined
    //macro function MEMORY_LOC_PFN does PAGE_SIZE * pfn (basically the macro function calculates the physical address)
    memset(MEMORY_LOC_OF_PFN(frame_number_to_be_allocated), 0, PAGE_SIZE); //The new frame might have been in use previously so we must reset
    // its contents

    //Step 2.) Update the frame table
    //we don't want to evict the frame containing the page table of a running process so this frame is protected
    frame_table[frame_number_to_be_allocated].protected = 0x1;
    frame_table[frame_number_to_be_allocated].mapped = 0x1;
    frame_table[frame_number_to_be_allocated].process = proc;
    frame_table[frame_number_to_be_allocated].vpn = 0; //0 since the first frame of every process
                                                                          //is its page table

    //Step 3.) Update the PCB
    proc->saved_ptbr = frame_number_to_be_allocated; //we DON'T want MEMORY_LOC_OF_PFN(pfn) here; we want just the pfn
}

/**
 * TODO 4: update any global vars and proc's PCB to match the context_switch.
 * --------------------------------- PROBLEM 4 --------------------------------------
 * Checkout PDF section 5 for this problem
 * 
 * Switches the currently running process to the process referenced by the proc 
 * argument.
 * 
 * Every process has its own page table, as you allocated in proc_init. You will
 * need to tell the processor to use the new process's page table.
 * 
 * @param proc pointer to process to become the currently running process.
 * 
 * HINTS:
 *      - Look at the global variables defined in pagesim.h. You may be interested in
 *      the definition of pcb_t as well.
 * ----------------------------------------------------------------------------------
 */
void context_switch(pcb_t *proc) {
    PTBR = proc->saved_ptbr; //switch the PTBR to the passed in processes PTBR
    current_process = proc; //set the current process to the new process . not sure if I need to do this here
}

/**
 * // TODO 8 : Iterate the proc's page table and clean up each valid page
 * --------------------------------- PROBLEM 8 --------------------------------------
 * Checkout PDF section 8 for this problem
 * 
 * When a process exits, you need to free any pages previously occupied by the
 * process.
 * 
 * HINTS:
 *      - If the process has swapped any pages to disk, you must call
 *      swap_free() using the page table entry pointer as a parameter.
 *      - If you free any protected pages, you must also clear their"protected" bits.
 * ----------------------------------------------------------------------------------
 */
void proc_cleanup(pcb_t *proc) {
    pte_t *pgtable = (pte_t *) MEMORY_LOC_OF_PFN(proc->saved_ptbr);


    for (size_t i = 0; i < NUM_PAGES; i++) {

        if (pgtable[i].valid && swap_exists(pgtable + i)) {
            swap_free(pgtable + i);
        }
        memset(pgtable + i, 0, sizeof(pte_t));


        //iterate through the frame table, checking if each frame table entry is of the same PID that the dead process's
        //PID is. If it is, we get the PFN of that frame table entry and go ahead and clear the memory in that physical
        //frame and clear the frame table entry.
        if (frame_table[i].process != NULL && //some frame table entries are not populated
            frame_table[i].process->pid == proc->pid) {
            (frame_table + i)->process = NULL;
            (frame_table + i)->protected = 0;
            (frame_table + i)->mapped = 0;
            (frame_table + i)->referenced = 0;
            (frame_table + i)->vpn = 0;
//            memset(frame_table + i, 0, sizeof(pfn_t));
        }
    }



}

#pragma GCC diagnostic pop
