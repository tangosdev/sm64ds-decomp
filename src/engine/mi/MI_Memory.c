/* CPU memory fill and copy (SDK mi_memory.c), .text 0x0205a448..0x0205a74c.
 *
 * HAND-ASM PRIMITIVE: every function in the SDK's mi_memory.c is an asm
 * function (ldm/stm block moves and the halfword-safe byte fill/copy
 * ladders), so the asm blocks are the faithful source.
 *
 * mwccarm emits an asm function where it stands (only C bodies are deferred
 * and come out reversed), so the file reads in ROM order. The linker names
 * are the repo's existing ones; the SDK name of each function, from
 * pret/pokediamond's mi_memory.c, is in its comment. MIi_CpuSend32 and
 * MI_Copy64B are not in this ROM (nothing called them, so the linker dropped
 * them).
 */
#include "types.h"

/* MIi_CpuClear16: fills nbytes bytes with the halfword val. */
asm void MultiStore16(unsigned short val, char *dst, int nbytes)
{
    mov r3, #0
loop:
    cmp r3, r2
    strlth r0, [r1, r3]
    addlt r3, r3, #2
    blt loop
    bx lr
}

/* MIi_CpuCopy16: copies size bytes one halfword at a time. */
asm void MultiCopyHalf(const void *src, void *dst, s32 size)
{
    mov ip, #0
loop:
    cmp ip, r2
    ldrlth r3, [r0, ip]
    strlth r3, [r1, ip]
    addlt ip, ip, #2
    blt loop
    bx lr
}

/* MIi_CpuClear32 */
asm void MultiStore_Int(int val, int *dst, int len)
{
    add ip, r1, r2
loop:
    cmp r1, ip
    stmltia r1!, {r0}
    blt loop
    bx lr
}

/* MIi_CpuCopy32: copies len bytes (a multiple of 4) one word at a time. */
asm void MultiCopy_Int(int *src, int *dst, int len)
{
    add ip, r1, r2
loop:
    cmp r1, ip
    ldmltia r0!, {r2}
    stmltia r1!, {r2}
    blt loop
    bx lr
}

/* MIi_CpuClearFast: 32 bytes per stm, then a word tail. */
asm void MultiStore32Bytes(unsigned val, int *dst, int len)
{
    stmdb sp!, {r4, r5, r6, r7, r8, r9}
    add r9, r1, r2
    mov ip, r2, lsr #5
    add ip, r1, ip, lsl #5
    mov r2, r0
    mov r3, r2
    mov r4, r2
    mov r5, r2
    mov r6, r2
    mov r7, r2
    mov r8, r2
loop32:
    cmp r1, ip
    stmltia r1!, {r0, r2, r3, r4, r5, r6, r7, r8}
    blt loop32
loop4:
    cmp r1, r9
    stmltia r1!, {r0}
    blt loop4
    ldmia sp!, {r4, r5, r6, r7, r8, r9}
    bx lr
}

/* MIi_CpuCopyFast: 32 bytes per ldm/stm, then a word tail. */
asm void MultiCopy32Bytes(int *src, int *dst, int len)
{
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10}
    add r10, r1, r2
    mov ip, r2, lsr #5
    add ip, r1, ip, lsl #5
loop32:
    cmp r1, ip
    ldmltia r0!, {r2, r3, r4, r5, r6, r7, r8, r9}
    stmltia r1!, {r2, r3, r4, r5, r6, r7, r8, r9}
    blt loop32
loop4:
    cmp r1, r10
    ldmltia r0!, {r2}
    stmltia r1!, {r2}
    blt loop4
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10}
    bx lr
}

/* MI_Copy32B */
asm void Copy32Bytes(const void *src, void *dst)
{
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3}
    stmia r1!, {r2, r3}
    bx lr
}

/* MI_Copy36B */
asm void Copy36Bytes(const void *src, void *dst)
{
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    bx lr
}

/* MI_Copy48B */
asm void Copy48Bytes(const void *src, void *dst)
{
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    ldmia r0!, {r2, r3, ip}
    stmia r1!, {r2, r3, ip}
    bx lr
}

/* MI_CpuFill8: the byte fill that pairs with CpuCopy8. It aligns to a
 * halfword and then a word with read-modify-write at the unaligned head and
 * tail (so the adjacent byte survives), replicates the fill byte across the
 * word, and runs a str-writeback word loop. */
asm void func_0205a588(void *dst, u8 data, u32 size)
{
    cmp r2, #0
    bxeq lr
    tst r0, #1
    beq L2c
    ldrh ip, [r0, #-1]
    and ip, ip, #0xff
    orr r3, ip, r1, lsl #8
    strh r3, [r0, #-1]
    add r0, r0, #1
    subs r2, r2, #1
    bxeq lr
L2c:
    cmp r2, #2
    blo L74
    orr r1, r1, r1, lsl #8
    tst r0, #2
    beq L4c
    strh r1, [r0], #2
    subs r2, r2, #2
    bxeq lr
L4c:
    orr r1, r1, r1, lsl #16
    bics r3, r2, #3
    beq L6c
    sub r2, r2, r3
    add ip, r3, r0
L60:
    str r1, [r0], #4
    cmp r0, ip
    blo L60
L6c:
    tst r2, #2
    strneh r1, [r0], #2
L74:
    tst r2, #1
    bxeq lr
    ldrh r3, [r0]
    and r3, r3, #0xff00
    and r1, r1, #0xff
    orr r1, r1, r3
    strh r1, [r0]
    bx lr
}

/* MI_CpuCopy8: an unaligned byte copy that never issues an 8-bit store. The
 * destination is aligned to a halfword with an edge-preserving read-modify-
 * write, then the copy runs as a halfword shift (when source and destination
 * parity differ) or a word+halfword bulk loop, closing with the same edge RMW
 * tail. */
asm void CpuCopy8(const void *src, void *dst, u32 size)
{
    cmp r2, #0
    bxeq lr
    tst r1, #1
    beq L40
    ldrh ip, [r1, #-1]
    and ip, ip, #0xff
    tst r0, #1
    ldrneh r3, [r0, #-1]
    movne r3, r3, lsr #8
    ldreqh r3, [r0]
    orr r3, ip, r3, lsl #8
    strh r3, [r1, #-1]
    add r0, r0, #1
    add r1, r1, #1
    subs r2, r2, #1
    bxeq lr
L40:
    eor ip, r1, r0
    tst ip, #1
    beq L94
    bic r0, r0, #1
    ldrh ip, [r0], #2
    mov r3, ip, lsr #8
    subs r2, r2, #2
    blo L78
L60:
    ldrh ip, [r0], #2
    orr ip, r3, ip, lsl #8
    strh ip, [r1], #2
    mov r3, ip, lsr #0x10
    subs r2, r2, #2
    bhs L60
L78:
    tst r2, #1
    bxeq lr
    ldrh ip, [r1]
    and ip, ip, #0xff00
    orr ip, ip, r3
    strh ip, [r1]
    bx lr
L94:
    tst ip, #2
    beq Lc0
    bics r3, r2, #1
    beq L10c
    sub r2, r2, r3
    add ip, r3, r1
Lac:
    ldrh r3, [r0], #2
    strh r3, [r1], #2
    cmp r1, ip
    blo Lac
    b L10c
Lc0:
    cmp r2, #2
    blo L10c
    tst r1, #2
    beq Le0
    ldrh r3, [r0], #2
    strh r3, [r1], #2
    subs r2, r2, #2
    bxeq lr
Le0:
    bics r3, r2, #3
    beq L100
    sub r2, r2, r3
    add ip, r3, r1
Lf0:
    ldr r3, [r0], #4
    str r3, [r1], #4
    cmp r1, ip
    blo Lf0
L100:
    tst r2, #2
    ldrneh r3, [r0], #2
    strneh r3, [r1], #2
L10c:
    tst r2, #1
    bxeq lr
    ldrh r2, [r1]
    ldrh r0, [r0]
    and r2, r2, #0xff00
    and r0, r0, #0xff
    orr r0, r2, r0
    strh r0, [r1]
    bx lr
}
