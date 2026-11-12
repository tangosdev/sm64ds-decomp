#ifndef NITRO_HW_MMAP_TCM_H
#define NITRO_HW_MMAP_TCM_H

/* DTCM layout constants, under NitroSDK's names (nitro/hw/ARM9/mmap_tcm.h).
 * The SVC stack sits at the top of DTCM, below the system reserve; a new
 * thread's context carves its SVC stack off the top of the thread's own. */

#define HW_SVC_STACK_SIZE 0x40

#endif /* NITRO_HW_MMAP_TCM_H */
