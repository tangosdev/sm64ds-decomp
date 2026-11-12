#ifndef NITRO_HW_ARMARCH_H
#define NITRO_HW_ARMARCH_H

/* The ARM program status register fields, under NitroSDK's names
 * (nitro/hw/common/armArch.h). The hand-written asm that switches CPU modes
 * (OS_InitContext, OS_SaveContext, OS_LoadContext) builds its CPSR values
 * from these. */

#define HW_PSR_CPU_MODE_MASK 0x1f

#define HW_PSR_USER_MODE 0x10
#define HW_PSR_FIQ_MODE 0x11
#define HW_PSR_IRQ_MODE 0x12
#define HW_PSR_SVC_MODE 0x13
#define HW_PSR_ABORT_MODE 0x17
#define HW_PSR_UNDEF_MODE 0x1b
#define HW_PSR_SYS_MODE 0x1f

#define HW_PSR_ARM_STATE 0x0
#define HW_PSR_THUMB_STATE 0x20

#define HW_PSR_FIQ_DISABLE 0x40
#define HW_PSR_IRQ_DISABLE 0x80
#define HW_PSR_IRQ_FIQ_DISABLE 0xc0

#endif /* NITRO_HW_ARMARCH_H */
