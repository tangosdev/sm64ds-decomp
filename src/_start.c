// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only runtime primitive).
// _start at 0x02004800 -- NitroSDK crt0, the ARM9 entry point the cartridge header names.
// It disables IME, runs init_cp15, switches CPSR modes (SVC/IRQ/SYS) to set up the mode
// stacks in DTCM, decompresses the static image and runs the autoloads, clears DTCM,
// palette and OAM, stores the interrupt-check word, runs _fp_init, NitroStartUp and the
// static initializers, then jumps to NitroMain with lr set to the reset vector. No C
// compiler emits msr or a bx with a pooled lr, so there is no C to decompile it to.
//
// pret/pokediamond's crt0.c has the same _start, from an SDK a little newer than this
// one: it clears with its own INITi_CpuClear32 where this calls the MI library's copy
// (MIi_CpuClear32, the same five instructions), and it aligns sp before the jump.
// 0x02004ad8 is _start_ModuleParams (BuildInfo, the name dsd needs); +0x14 is the end of
// the compressed static image.
#include "nitro/mi/memory.h"
extern void init_cp15(void);
extern void MIi_UncompressBackward(void *bottom);
extern void do_autoload(void);
extern void _fp_init(void);
extern void NitroStartUp(void);
extern void __call_static_initializers(void);
asm void _start(void) {
    mov     ip, #0x4000000
    str     ip, [ip, #0x208]
    bl      init_cp15
    mov     r0, #0x13
    msr     cpsr_c, r0
    ldr     r0, =0x023c0000
    add     r0, r0, #0x3fc0
    mov     sp, r0
    mov     r0, #0x12
    msr     cpsr_c, r0
    ldr     r0, =0x023c0000
    add     r0, r0, #0x3fc0
    sub     r0, r0, #0x40
    mov     sp, r0
    ldr     r1, =0x600
    sub     r1, r0, r1
    mov     r0, #0x1f
    msr     cpsr_fsxc, r0
    sub     sp, r1, #4
    ldr     r1, =0x2004ad8
    ldr     r0, [r1, #0x14]
    bl      MIi_UncompressBackward
    bl      do_autoload
    mov     r0, #0
    ldr     r1, =0x023c0000
    mov     r2, #0x4000
    bl      MIi_CpuClear32
    mov     r0, #0
    ldr     r1, =0x5000000
    mov     r2, #0x400
    bl      MIi_CpuClear32
    mov     r0, #0x200
    ldr     r1, =0x7000000
    mov     r2, #0x400
    bl      MIi_CpuClear32
    ldr     r1, =0x023c0000
    add     r1, r1, #0x3fc0
    add     r1, r1, #0x3c
    ldr     r0, =0x1ffd97c
    str     r0, [r1]
    bl      _fp_init
    bl      NitroStartUp
    bl      __call_static_initializers
    ldr     r1, =0x2007000
    ldr     lr, =0xffff0000
    bx      r1
}
