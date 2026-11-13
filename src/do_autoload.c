// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only runtime primitive).
//
// do_autoload at 0x0200497c -- NitroSDK crt0. 0x02004ad8 is _start_ModuleParams: its
// first three words are the autoload list, the list end and the packed autoload data.
// For each {destination, size, bss size} entry it copies the block and zeroes the bss
// after it; then it zeroes the static bss (params +0x0c..+0x10) and falls into
// _start_AutoloadDoneCallback (AutoloadCallback, the name dsd needs) with a plain `b`,
// r0 still pointing at the params.
//
// pret/pokediamond's crt0.c has the same loop as an `asm` function, plus a cache-line
// flush per block that this older SDK does not do. It uses r4-r7 without saving them and
// never returns by itself, so there is no C to write. Per asm policy.
extern void AutoloadCallback(void *argv[]);
asm void do_autoload(void) {
    ldr     r0, =0x2004ad8
    ldr     r1, [r0]
    ldr     r2, [r0, #4]
    ldr     r3, [r0, #8]
outer:
    cmp     r1, r2
    beq     tail
    ldr     r4, [r1], #4
    ldr     r5, [r1], #4
    add     r6, r4, r5
in1:
    cmp     r4, r6
    ldrmi   r7, [r3], #4
    strmi   r7, [r4], #4
    bmi     in1
    ldr     r5, [r1], #4
    add     r6, r4, r5
    mov     r7, #0
in2:
    cmp     r4, r6
    strlo   r7, [r4], #4
    blo     in2
    beq     outer
tail:
    ldr     r4, [r0, #0xc]
    ldr     r6, [r0, #0x10]
    mov     r7, #0
in3:
    cmp     r4, r6
    strlo   r7, [r4], #4
    blo     in3
    b       AutoloadCallback
}
