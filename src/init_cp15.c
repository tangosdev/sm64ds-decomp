/* init_cp15 at 0x020049f0 -- NitroSDK crt0, the first call _start makes: the
 * boot-time MPU and cache configuration, and the largest CP15 routine by an
 * order of magnitude.
 *
 * The sequence, for a reader who has not met CP15 before: disable the caches
 * and MPU by clearing bits in the control register; invalidate both caches and
 * drain the write buffer; program all eight MPU data regions (c6,c0 through
 * c6,c7) with packed base/size/enable words; place the DTCM at
 * SDK_AUTOLOAD_DTCM_START with a 32-entry ITCM (c9,c1); set the cacheable,
 * bufferable and access permission masks (c2, c3, c5); then re-enable
 * everything by ORing 0x5707d back into the control register.
 *
 * pret/pokediamond's crt0.c has the same steps as `static asm void
 * init_cp15(void)`, right after _start_AutoloadDoneCallback. The ROM keeps
 * that order: _start, MIi_UncompressBackward, do_autoload,
 * AutoloadCallback (the SDK's _start_AutoloadDoneCallback), then this, then
 * BuildInfo (_start_ModuleParams). It cannot be static here because _start
 * lives in its own source.
 *
 * HAND-ASM PRIMITIVE: `asm void' rather than an `asm { }' block inside an
 * ordinary function because the ROM has no prologue: the function begins
 * directly at `mrc' and runs to its literal pool, so the terminating `bx lr' is
 * written out here instead of being left to the compiler. */

/* The DTCM base, the linker's SDK_AUTOLOAD_DTCM_START (0x023c0000 here):
   init_cp15 is what puts the DTCM there. */
#include "nitro/hw/mmap_global.h"

asm void init_cp15(void)
{
    mrc p15, 0, r0, c1, c0, 0
    ldr r1, =0xf9005
    bic r0, r0, r1
    mcr p15, 0, r0, c1, c0, 0
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    mcr p15, 0, r0, c7, c6, 0
    mcr p15, 0, r0, c7, c10, 4
    ldr r0, =0x4000033
    mcr p15, 0, r0, c6, c0, 0
    ldr r0, =0x200002b
    mcr p15, 0, r0, c6, c1, 0
    ldr r0, =0x27c0023
    mcr p15, 0, r0, c6, c2, 0
    ldr r0, =0x8000035
    mcr p15, 0, r0, c6, c3, 0
    ldr r0, =SDK_AUTOLOAD_DTCM_START
    orr r0, r0, #0x1a
    orr r0, r0, #1
    mcr p15, 0, r0, c6, c4, 0
    ldr r0, =0x100002f
    mcr p15, 0, r0, c6, c5, 0
    ldr r0, =0xffff001d
    mcr p15, 0, r0, c6, c6, 0
    ldr r0, =0x27ff017
    mcr p15, 0, r0, c6, c7, 0
    mov r0, #0x20
    mcr p15, 0, r0, c9, c1, 1
    ldr r0, =SDK_AUTOLOAD_DTCM_START
    orr r0, r0, #0xa
    mcr p15, 0, r0, c9, c1, 0
    mov r0, #0x42
    mcr p15, 0, r0, c2, c0, 1
    mov r0, #0x42
    mcr p15, 0, r0, c2, c0, 0
    mov r0, #2
    mcr p15, 0, r0, c3, c0, 0
    ldr r0, =0x5100011
    mcr p15, 0, r0, c5, c0, 3
    ldr r0, =0x15111011
    mcr p15, 0, r0, c5, c0, 2
    mrc p15, 0, r0, c1, c0, 0
    ldr r1, =0x5707d
    orr r0, r0, r1
    mcr p15, 0, r0, c1, c0, 0
    bx lr
}
