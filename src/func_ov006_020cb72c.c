// HAND-ASM: pure regperm floor on 6 words under mwccarm 1.2/sp2p3 C.
// Structure/schedule matches near-miss tip (div=6); only short-index coloring
// differs (ROM ldrsh ip + table load to r1 vs C ldrsh r1 + table load to r0).
// Exhausted 20+ C variants + permuter 350+ iters score floor 40 (regperm).
// Same class as func_ov007_020bfd70 / func_02058568 (regperm → asm hatch).
extern int func_ov006_020cb528(void);
extern int func_ov006_020cc8c8(void);
extern int func_ov006_020e6e3c(int a, int b);
extern void func_ov006_020ccc8c(char *c);
extern int data_ov006_021405c8[];
extern short data_ov006_02140538;

asm void func_ov006_020cb72c(void)
{
    stmdb sp!, {r4, lr}
    mov r4, r0
    ldr r1, [r4, #0x1c]
    cmp r1, #0
    ble L74
    ldrsh r12, [r4, #0x52]
    ldr r2, [pc, #0xc0]
    ldr r3, [r4, #0x20]
    ldr r1, [r2, r12, lsl #2]
    cmp r3, r1
    bgt L58
    ldr r1, [pc, #0xb0]
    ldrsh r1, [r1]
    cmp r12, r1
    bne L44
    bl func_ov006_020cb528
    b L48
L44:
    bl func_ov006_020cc8c8
L48:
    ldr r1, [r4, #0x1c]
    mov r0, #0x1b4
    bl func_ov006_020e6e3c
    b La0
L58:
    ldr r0, [r2, #4]
    add r0, r0, #0x30000
    cmp r3, r0
    blt La0
    cmp r12, #1
    streq r0, [r4, #0x20]
    b La0
L74:
    ldr r0, [pc, #0x64]
    ldr r2, [r4, #0x20]
    ldr r0, [r0]
    cmp r2, r0
    bgt La0
    mov r0, #0x1b4
    bl func_ov006_020e6e3c
    mov r0, r4
    bl func_ov006_020ccc8c
    ldmia sp!, {r4, lr}
    bx lr
La0:
    ldr r1, [r4, #0x1c]
    cmp r1, #0x70000
    movgt r0, #0x70000
    strgt r0, [r4, #0x1c]
    movgt r0, #0
    strgt r0, [r4, #0x34]
    ldmgtia sp!, {r4, lr}
    bxgt lr
    mov r0, #0x70000
    rsb r0, r0, #0
    cmp r1, r0
    strlt r0, [r4, #0x1c]
    movlt r0, #0
    strlt r0, [r4, #0x34]
    ldmia sp!, {r4, lr}
    bx lr
    dcd data_ov006_021405c8
    dcd data_ov006_02140538
}
