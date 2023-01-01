#include "../simulator-src/stats.h"
//#include "stats.h"

/* The stats. See the definition in stats.h. */
stats_t stats;

/**
 * TODO 10: Calculate AMAT
 * --------------------------------- PROBLEM 10 --------------------------------------
 * Checkout PDF section 10 for this problem
 *
 * accesses - The total number of accesses to the memory system
 * page faults - Accesses that resulted in a page fault
 * writebacks - How many times you wrote to disk
 * AMAT - The average memory access time of the memory system
 *
 * 
 * Calculate the total average time it takes for an access
 * 
 * HINTS:
 * 		- You may find the #defines in the stats.h file useful.
 * 		- You will need to include code to increment many of these stats in
 * 		the functions you have written for other parts of the project.
 * -----------------------------------------------------------------------------------
 */
void compute_stats() {
    stats.amat = ((double) (stats.page_faults * DISK_PAGE_READ_TIME +
                            stats.writebacks * DISK_PAGE_WRITE_TIME +
                            stats.accesses * MEMORY_ACCESS_TIME)) / ( (double) stats.accesses);
}
