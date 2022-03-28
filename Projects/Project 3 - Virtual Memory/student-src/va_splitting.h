#pragma once

#include "../simulator-src/mmu.h"

//#include "mmu.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"



/**
 * --------------------------------- PROBLEM 1 --------------------------------------
 * Checkout PDF Section 3 For this Problem
 *
 * Split the virtual address into its virtual page number and offset.
 * 
 * HINT: 
 *      -Examine the global defines in pagesim.h, which will be necessary in 
 *      implementing these functions.
 * ----------------------------------------------------------------------------------
 */

/**
 * TODO 1.1: return the VPN from virtual address addr.
 * @param addr virtual address
 * @return the page offset (last 14-bits)
 */
static inline vpn_t vaddr_vpn(vaddr_t addr) {
    return addr >> OFFSET_LEN;
}

/**
 * TODO 1.2: return the offset into the frame from virtual address addr.
 *
 * @param addr virtual address
 * @return virtual page number (VPN) (first 10-bits)
 */
static inline uint16_t vaddr_offset(vaddr_t addr) {
    unsigned int bit_mask = 16383; // 0b000000000011111111111111;
    return addr & bit_mask;
}

#pragma GCC diagnostic pop