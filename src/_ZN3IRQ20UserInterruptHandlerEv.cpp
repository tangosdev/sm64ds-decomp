//cpp
// @symbol _ZN3IRQ20UserInterruptHandlerEv
// HAND-ASM PRIMITIVE. Nintendo's own NitroSDK source ships this exact routine as
// `asm void OS_IrqHandler(void)` in arm9/lib/NitroSDK/src/OS_irqHandler.c (lines
// 10-34 of the pokediamond decomp, one `#pragma section ITCM` above it), instruction
// for instruction identical to this function:
//
//   stmfd sp!, {lr}
//   mov ip, #0x04000000
//   add ip, ip, #0x210
//   ldr r1, [ip, #-8]
//   cmp r1, #0
//   ldmeqfd sp!, {pc}
//   ldmia ip, {r1, r2}
//   ands r1, r1, r2
//   ldmeqfd sp!, {pc}
//   mov r3, #0x80000000
// _01FF8028:
//   clz r0, r1
//   bics r1, r1, r3, lsr r0
//   bne _01FF8028
//   mov r1, r3, lsr r0
//   str r1, [ip, #0x4]
//   rsbs r0, r0, #0x1f
//   ldr r1, =OS_IRQTable
//   ldr r0, [r1, r0, lsl #2]
//   ldr lr, =OS_IrqHandler_ThreadSwitch
//   bx r0
//
// The SDK's OS_IRQTable is this game's data_02099fe4 (the same array
// IRQ::GetIRQHandler/IRQ::SetIRQHandler index; slot 0 is IRQ::EmptyHandler), and
// OS_IrqHandler_ThreadSwitch is this game's func_01ffd9d4, already a HAND-ASM
// PRIMITIVE in this tree and the byte-exact continuation the same SDK file
// carries as a second `asm` function immediately below this one (lines 36-127).
// The tail here hand-loads lr with func_01ffd9d4's address and bx's into the
// table entry, so func_01ffd9d4's `ldr pc, [sp], #4` exits pop THIS function's
// pushed lr instead of returning here -- a cross-function stack-frame handoff
// no C compiler emits, matching the SDK's own choice to hand-write both halves.
//
// Not reachable from C on this toolchain regardless: mwccarm 2004/b56 has no
// `clz` intrinsic (__clz, __builtin_clz, __CLZ, __CLZ32, _CountLeadingZeros and
// __rt_clz all compile to an external long-call veneer, never the clz opcode),
// so a portable leading-zero loop compiles to 0xa8 bytes against the ROM's 0x58.
#include "IRQ.h"

extern "C" {
extern IRQ::Handler data_02099fe4[];
extern void func_01ffd9d4(void);
}

namespace IRQ {

asm void UserInterruptHandler(void)
{
    stmdb sp!, {lr}
    mov ip, #0x04000000
    add ip, ip, #0x210
    ldr r1, [ip, #-8]
    cmp r1, #0
    ldmeqia sp!, {pc}
    ldmia ip, {r1, r2}
    ands r1, r1, r2
    ldmeqia sp!, {pc}
    mov r3, #0x80000000
_loop:
    clz r0, r1
    bics r1, r1, r3, lsr r0
    bne _loop
    mov r1, r3, lsr r0
    str r1, [ip, #0x4]
    rsbs r0, r0, #0x1f
    ldr r1, =data_02099fe4
    ldr r0, [r1, r0, lsl #2]
    ldr lr, =func_01ffd9d4
    bx r0
}

}
