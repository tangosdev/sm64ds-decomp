#ifndef NITRO_HW_MMAP_TCM_H
#define NITRO_HW_MMAP_TCM_H

/* DTCM layout constants, under NitroSDK's names (nitro/hw/ARM9/mmap_tcm.h).
 * The top 0x40 bytes of DTCM are the system reserve; the SVC stack sits
 * below it and the IRQ stack below that. A new thread's context carves its
 * SVC stack off the top of the thread's own. */

#include "nitro/hw/mmap_global.h"

#define HW_SVC_STACK_SIZE  0x40
#define HW_DTCM_SYSRV_SIZE 0x40

#define HW_DTCM_IRQ_STACK_END (HW_DTCM_SVC_STACK)
#define HW_DTCM_SVC_STACK     (HW_DTCM_SVC_STACK_END - HW_SVC_STACK_SIZE)
#define HW_DTCM_SVC_STACK_END (HW_DTCM + 0x00003fc0)

#define HW_DTCM_SYSRV_OFS_DEBUGGER    0x00
#define HW_DTCM_SYSRV_OFS_EXCP_VECTOR 0x1c
#define HW_DTCM_SYSRV_OFS_RESERVED    0x20
#define HW_DTCM_SYSRV_OFS_INTR_CHECK  0x38
#define HW_DTCM_SYSRV_OFS_INTR_VECTOR 0x3c

#define HW_DTCM_SYSRV      (HW_DTCM + 0x00003fc0)
#define HW_EXCP_VECTOR_BUF (HW_DTCM_SYSRV + HW_DTCM_SYSRV_OFS_EXCP_VECTOR)
#define HW_INTR_CHECK_BUF  (HW_DTCM_SYSRV + HW_DTCM_SYSRV_OFS_INTR_CHECK)
#define HW_INTR_VECTOR_BUF (HW_DTCM_SYSRV + HW_DTCM_SYSRV_OFS_INTR_VECTOR)

#endif /* NITRO_HW_MMAP_TCM_H */
