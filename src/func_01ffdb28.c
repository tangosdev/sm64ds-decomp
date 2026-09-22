// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ffdb28 at 0x01ffdb28 (ITCM) -- the last step of the soft reset described in
// src/func_01ffdd98.c: after func_01ffdd08 has reloaded both binaries off the card,
// this hands off to the freshly loaded ARM9 image and never returns.
//
// Why this is assembly and not C, independent of the instruction list:
//   * It makes two `bl func_02059d98` calls with NO prologue and NO frame, and keeps
//     live values in r1 and ip straight across them -- the saved word at 0x027ffd9c
//     is held in ip over the first call, and the REG_IPCSYNC pointer in r1 over both.
//     That is only sound because the callee (src/func_02059d98.c, a hand-asm block
//     clear) is known to touch r0-r10 and nothing else. No compiler assumes a
//     callee's clobber set; mwccarm would spill to callee-saved registers and push.
//   * The tail is a bulk register load, `ldmia r11, {r0-r10}`, followed by `mov r11, #0`
//     and `bx ip` -- eleven registers set up as the entry state of another program and
//     then jumped to. C has no construct that assigns the register file and transfers.
//   * It never returns, so it never needs lr; `mov lr, ip` seeds the new image's lr
//     with its own entry point instead.
//
// The handshake: wait until the ARM7 puts 1 in REG_IPCSYNC's input nibble, answer with
// 1 in the output nibble (0x100), clear the two 0x80-byte scratch blocks at 0x027ffd80
// and 0x027fff80 -- preserving the word at 0x027ffd9c across the first clear -- then
// wait for the ARM7 to drop its 1 and release the register. 0x027ffe24 is the ARM9
// entry-address field of the cartridge header copy at 0x027ffe00, the same block
// src/func_01ffdd08.c reads its ROM offset / RAM address / size triples out of.
//
// 0x04000208 is REG_IME; storing 0x04000000 there clears bit 0, i.e. interrupts off.
// The I/O base doubles as both the value and the store's base register.
// data_023c0000 is the DTCM block src/func_02057000.c also writes; +0x3ffc is its last
// word, and the same pool entry is reloaded as the register-image pointer at the end.

extern short data_023c0000[];
extern void func_02059d98(void *p, unsigned int n);

asm void func_01ffdb28(void)
{
    mov ip, #0x4000000
    str ip, [ip, #0x208]
    ldr r1, =data_023c0000
    add r1, r1, #0x3fc0
    add r1, r1, #0x3c
    mov r0, #0
    str r0, [r1]
    ldr r1, =0x04000180
_wait_ack:
    ldrh r0, [r1]
    and r0, r0, #0xf
    cmp r0, #1
    bne _wait_ack
    mov r0, #0x100
    strh r0, [r1]
    ldr r0, =0x027ffd9c
    ldr ip, [r0]
    ldr r0, =0x027ffd80
    mov r1, #0x80
    bl func_02059d98
    ldr r0, =0x027ffd9c
    str ip, [r0]
    ldr r0, =0x027fff80
    mov r1, #0x80
    bl func_02059d98
    ldr r1, =0x04000180
_wait_release:
    ldrh r0, [r1]
    and r0, r0, #0xf
    cmp r0, #1
    beq _wait_release
    mov r0, #0
    strh r0, [r1]
    ldr r3, =0x027ffe00
    ldr ip, [r3, #0x24]
    mov lr, ip
    ldr r11, =data_023c0000
    ldmia r11, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10}
    mov r11, #0
    bx ip
}
