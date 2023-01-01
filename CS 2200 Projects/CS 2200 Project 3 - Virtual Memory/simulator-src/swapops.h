#pragma once

/*
 * If the evicted page is dirty, you will need to write its contents to disk. The area of the disk that stores
 * the evicted pages is called the swap space. Swap space effectively extends the main memory (RAM)
 * of your system. If physical memory is full, the operating system kicks some frames to the hard disk to
 * accommodate others. When the “swapped” frames are needed again, they are restored from the disk into
 * physical memory. Without the swapping mechanism, when the system runs out of RAM, and we start evicting
 * physical frames, we lose the data stored in these frames, and the process whose pages were originally mapped
 * to the evicted frames loses its data forever. Therefore, upon selecting a victim, we need to make sure that
 * its data is swapped out to disk and restored when needed.
 */

#include "mmu.h"
#include "proc.h"
#include "pagesim.h"
//#include "paging.h"
#include "swap.h"
#include "types.h"

extern swap_queue_t swap_queue;

/**
 * Determines if the given page table entry has a swap entry; i.e. checks if the faulting page
 * was swapped out to disk previously.
 *
 * @param entry a pointer to the page table entry to check
 */
static inline int swap_exists(pte_t *entry) {
    return entry->swap_id != 0;
}

void swap_read(pte_t *entry, void *dst);
void swap_write(pte_t *entry, void *src);
void swap_free(pte_t *entry);

