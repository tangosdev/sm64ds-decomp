// HAND-ASM PRIMITIVE: byte-faithful asm-block match. MSL debug printf / format
// core (same family as pret MSL_Common_printf). Soft-double helpers of this
// cluster matched as C; this 0x1360 DFA stays asm — stack-home wall under C.
// Counts as matched (asm-primitive policy), not a C transcription.
extern int func_0206ca44();
extern int func_0206c9f4();
extern int func_0206c93c();
extern int func_0206c8b4();
extern int func_0206c244();
extern int func_0206bdb4();
extern int func_0206c51c();
extern int func_0206bc8c();
extern char data_0209a0a0;
extern char data_0209a130;
extern char data_02086894;
asm int func_0206a928(int a, int b, int c, int d) {
    stmdb sp!, {r4-r11, lr}
    sub sp, sp, #0x1dc
    mov r11, r0
    mov r0, #2
    str r0, [sp, #0xc]
    mov r0, #0xa
    str r0, [sp, #0xac]
    mov r0, #0x10
    str r0, [sp, #0xc8]
    mov r0, #7
    str r0, [sp, #0xa0]
    mov r0, #0x27
    str r0, [sp, #0xc4]
    mov r0, #8
    str r0, [sp, #0xb0]
    ldr r0, [sp, #0xc]
    mov r8, #0
    str r0, [sp, #0xc0]
    mov r0, #0x3c
    str r0, [sp, #0xcc]
    mov r0, #0x3a
    str r0, [sp, #0xd0]
    mov r0, #0x20
    str r0, [sp, #0xfc]
    mov r0, #0x2b
    str r0, [sp, #0xf8]
    mov r0, #0x2d
    str r0, [sp, #0xf4]
    mov r0, #0x30
    str r0, [sp, #0xd8]
    mov r0, #0x62
    str r0, [sp, #0xec]
    ldr r0, [sp, #0xc]
    mvn r3, #0
    str r0, [sp, #0xf0]
    mov r0, #0x58
    str r0, [sp, #0xe4]
    ldr r0, [sp, #0xc]
    str r3, [sp, #0x28]
    str r0, [sp, #0xe8]
    mov r0, #0x78
    str r0, [sp, #0xdc]
    ldr r0, [sp, #0xc]
    str r1, [sp]
    str r0, [sp, #0xe0]
    mov r0, #6
    str r0, [sp, #0x9c]
    mov r0, #9
    str r0, [sp, #0xb4]
    mov r0, #0x45
    str r0, [sp, #0x130]
    mov r0, #0x65
    str r0, [sp, #0x12c]
    ldr r0, [sp, #0xc]
    mov r9, r2
    str r0, [sp, #0x8c]
    mov r0, #3
    str r0, [sp, #0x90]
    mov r0, #5
    str r0, [sp, #0x98]
    mov r0, #0xc
    str r0, [sp, #0xbc]
    mov r0, #0xb
    str r0, [sp, #0xb8]
    ldr r0, [sp, #0xc]
    str r8, [sp, #0x20]
    str r0, [sp, #0xa8]
    mov r0, #4
    str r0, [sp, #0x94]
    mov r0, r3
    mov r7, r3
    mov r4, r8
    str r8, [sp, #0x24]
    mov r10, #1
    str r8, [sp, #0xd4]
    str r8, [sp, #0x100]
    str r8, [sp, #0x108]
    str r8, [sp, #0x110]
    str r8, [sp, #0x114]
    str r8, [sp, #0x104]
    str r8, [sp, #0x10c]
    str r8, [sp, #0x118]
    str r8, [sp, #0x11c]
    str r8, [sp, #0x120]
    str r8, [sp, #0x124]
    str r8, [sp, #0x128]
    str r8, [sp, #0x134]
    str r8, [sp, #0x138]
    str r8, [sp, #0x13c]
    str r8, [sp, #0x140]
    str r8, [sp, #0x144]
    str r8, [sp, #0x148]
    str r8, [sp, #0x84]
    str r8, [sp, #0xa4]
    str r8, [sp, #0x88]
    str r8, [sp, #0x74]
    str r8, [sp, #0x78]
    str r0, [sp, #0x7c]
    str r8, [sp, #0x80]
    str r8, [sp, #0x14c]
L_190:
    ldr r0, [sp]
    ands r0, r0, #1
    ldreq r0, [sp]
    ldreqh r0, [r0]
    andeq r1, r0, #0xff
    ldrne r0, [sp]
    ldrneh r0, [r0, #-1]
    andne r0, r0, #0xff00
    movne r1, r0, asr #8
    cmp r1, #0
    beq L_1334
    ldr r0, [sp]
    cmp r4, #0
    add r0, r0, #1
    str r0, [sp]
    bne L_288
    cmp r1, #0x25
    bne L_218
    ldr r0, [sp, #0x24]
    cmp r0, #0
    beq L_200
    ldr r2, [sp, #0x24]
    ldr r3, [r11, #0x10]
    mov r0, r11
    add r1, sp, #0x15c
    blx r3
    ldr r0, [sp, #0x74]
    str r0, [sp, #0x24]
L_200:
    ldr r8, [sp, #0x78]
    mov r4, r10
    mov r0, r8
    str r0, [sp, #0x28]
    ldr r7, [sp, #0x7c]
    b L_190
L_218:
    ldr r0, [sp, #0x24]
    add r2, sp, #0x15c
    add r0, r2, r0
    ands r2, r0, #1
    ldreqh r2, [r0]
    andeq r1, r1, #0xff
    andeq r2, r2, #0xff00
    orreq r1, r2, r1
    streqh r1, [r0]
    beq L_254
    ldrh r2, [r0, #-1]
    and r1, r1, #0xff
    and r2, r2, #0xff
    orr r1, r2, r1, lsl #8
    strh r1, [r0, #-1]
L_254:
    ldr r0, [sp, #0x24]
    add r0, r0, #1
    str r0, [sp, #0x24]
    cmp r0, #0x40
    bne L_190
    ldr r2, [sp, #0x24]
    ldr r3, [r11, #0x10]
    mov r0, r11
    add r1, sp, #0x15c
    blx r3
    ldr r0, [sp, #0x80]
    str r0, [sp, #0x24]
    b L_190
L_288:
    ldr r0, [pc, #0xff4]
    sub r3, r4, #1
    mov r2, #0x12
    mla r2, r3, r2, r0
    ldr r0, [pc, #0xfe8]
    mov r3, r1, lsl #1
    ldrh r0, [r0, r3]
    and r0, r0, #0xff
    mov r0, r0, lsl #1
    ldrh r0, [r0, r2]
    and r0, r0, #0xff
    cmp r0, #7
    addls pc, pc, r0, lsl #2
    b L_554
    b L_554
    b L_2e0
    b L_354
    b L_3c8
    b L_3f4
    b L_3fc
    b L_41c
    dcd 0xea000050
L_2e0:
    ldr r0, [sp, #0x24]
    add r2, sp, #0x15c
    add r0, r2, r0
    ands r2, r0, #1
    ldreqh r2, [r0]
    andeq r1, r1, #0xff
    andeq r2, r2, #0xff00
    orreq r1, r2, r1
    streqh r1, [r0]
    beq L_31c
    ldrh r2, [r0, #-1]
    and r1, r1, #0xff
    and r2, r2, #0xff
    orr r1, r2, r1, lsl #8
    strh r1, [r0, #-1]
L_31c:
    ldr r0, [sp, #0x24]
    add r0, r0, #1
    str r0, [sp, #0x24]
    cmp r0, #0x40
    bne L_34c
    ldr r2, [sp, #0x24]
    ldr r3, [r11, #0x10]
    mov r0, r11
    add r1, sp, #0x15c
    blx r3
    ldr r0, [sp, #0x84]
    str r0, [sp, #0x24]
L_34c:
    ldr r4, [sp, #0x88]
    b L_554
L_354:
    cmp r1, #0x2b
    ldr r4, [sp, #0x8c]
    bgt L_38c
    cmp r1, #0x2b
    bge L_3b0
    cmp r1, #0x23
    bgt L_554
    cmp r1, #0x20
    blt L_554
    cmp r1, #0x20
    beq L_3c0
    cmp r1, #0x23
    orreq r8, r8, #0x10
    b L_554
L_38c:
    cmp r1, #0x30
    bgt L_554
    cmp r1, #0x2d
    blt L_554
    cmp r1, #0x2d
    beq L_3b8
    cmp r1, #0x30
    orreq r8, r8, #4
    b L_554
L_3b0:
    orr r8, r8, #2
    b L_554
L_3b8:
    orr r8, r8, #1
    b L_554
L_3c0:
    orr r8, r8, #8
    b L_554
L_3c8:
    ldr r0, [sp, #0x28]
    mvn r2, #0
    cmp r0, r2
    subeq r0, r1, #0x30
    subne r2, r1, #0x30
    movne r1, #0xa
    streq r0, [sp, #0x28]
    mlane r0, r1, r0, r2
    ldr r4, [sp, #0x90]
    strne r0, [sp, #0x28]
    b L_554
L_3f4:
    ldr r4, [sp, #0x94]
    b L_554
L_3fc:
    mvn r0, #0
    cmp r7, r0
    subeq r7, r1, #0x30
    subne r1, r1, #0x30
    movne r0, #0xa
    mlane r7, r0, r7, r1
    ldr r4, [sp, #0x98]
    b L_554
L_41c:
    ldr r4, [sp, #0x9c]
    b L_554
    dcd 0xe3510058
    dcd 0xe59d40a0
    dcd 0xca00000b
    dcd 0xe3510058
    dcd 0xaa000038
    dcd 0xe3510047
    dcd 0xca000044
    dcd 0xe3510045
    dcd 0xba000042
    dcd 0xe3510045
    dcd 0x0a00003b
    dcd 0xe3510047
    dcd 0x059d00bc
    dcd 0x058d000c
    dcd 0xea00003c
    dcd 0xe3510069
    dcd 0xca00000a
    dcd 0xe2510062
    dcd 0x508ff100
    dcd 0xea000037
    dcd 0xea000014
    dcd 0xea000016
    dcd 0xea00001a
    dcd 0xea00002b
    dcd 0xea000027
    dcd 0xea00002f
    dcd 0xea000030
    dcd 0xea000015
    dcd 0xe241006f
    dcd 0xe3500009
    dcd 0x908ff100
    dcd 0xea00002b
    dcd 0xea000016
    dcd 0xea000029
    dcd 0xea000028
    dcd 0xea000027
    dcd 0xea000009
    dcd 0xea000025
    dcd 0xea00000d
    dcd 0xea000023
    dcd 0xea000022
    dcd 0xea000010
    dcd 0xe59d0094
    dcd 0xe58d000c
    dcd 0xea00001e
    dcd 0xe58da00c
    dcd 0xea00001c
    dcd 0xe59d00a4
    dcd 0xe58d000c
    dcd 0xea000019
    dcd 0xe59d00a8
    dcd 0xe58d000c
    dcd 0xea000016
    dcd 0xe59d0090
    dcd 0xe58d000c
    dcd 0xea000013
    dcd 0xe59d0098
    dcd 0xe58d000c
    dcd 0xea000010
    dcd 0xe59d009c
    dcd 0xe58d000c
    dcd 0xea00000d
    dcd 0xe1a00004
    dcd 0xe58d000c
    dcd 0xea00000a
    dcd 0xe59d00ac
    dcd 0xe58d000c
    dcd 0xea000007
    dcd 0xe59d00b0
    dcd 0xe58d000c
    dcd 0xea000004
    dcd 0xe59d00b4
    dcd 0xe58d000c
    dcd 0xea000001
    dcd 0xe59d00b8
    dcd 0xe58d000c
L_554:
    cmp r4, #7
    bne L_190
    ldr r0, [sp, #0xc]
    cmp r0, #0xc
    addls pc, pc, r0, lsl #2
    b L_132c
    b L_5d0
    b L_5a0
    b L_654
    b L_684
    b L_6a8
    b L_6cc
    b L_6f0
    b L_71c
    b L_a08
    b L_a08
    b L_a08
    b L_a08
    dcd 0xea000119
L_5a0:
    ldr r1, [r9]
    mov r0, r11
    add r1, r1, #4
    str r1, [r9]
    ldr r2, [r9]
    add r1, sp, #0x150
    ldr r3, [r2, #-4]
    mov r2, r10
    str r3, [sp, #0x150]
    ldr r3, [r11, #0x10]
    blx r3
    b L_132c
L_5d0:
    ldr r0, [r9]
    add r0, r0, #4
    str r0, [r9]
    ldr r0, [r9]
    ldr r5, [r0, #-4]
    cmp r5, #0
    ldreq r5, [pc, #0xc9c]
    mov r0, r5
    bl func_0206ca44
    mov r4, r0
    ands r6, r8, #1
    bne L_61c
    ldr r0, [sp, #0x28]
    cmp r4, r0
    bge L_61c
    ldr r1, [sp, #0x28]
    mov r0, r11
    sub r1, r1, r4
    bl func_0206c9f4
L_61c:
    mov r1, r5
    mov r0, r11
    mov r2, r4
    ldr r3, [r11, #0x10]
    blx r3
    cmp r6, #0
    beq L_132c
    ldr r0, [sp, #0x28]
    cmp r4, r0
    bge L_132c
    sub r1, r0, r4
    mov r0, r11
    bl func_0206c9f4
    b L_132c
L_654:
    ldr r0, [r9]
    add r0, r0, #4
    str r0, [r9]
    ldr r0, [r9]
    ldr r6, [r0, #-4]
    ldr r0, [sp, #0xac]
    cmp r6, #0
    orrlt r8, r8, #0x4000
    rsblt r6, r6, #0
    orr r8, r8, #0x1000
    str r0, [sp, #0x1c]
    b L_744
L_684:
    ldr r0, [r9]
    orr r8, r8, #0x2000
    add r0, r0, #4
    str r0, [r9]
    ldr r1, [r9]
    ldr r0, [sp, #0xac]
    ldr r6, [r1, #-4]
    str r0, [sp, #0x1c]
    b L_744
L_6a8:
    ldr r0, [r9]
    orr r8, r8, #0x2000
    add r0, r0, #4
    str r0, [r9]
    ldr r1, [r9]
    ldr r0, [sp, #0xc0]
    ldr r6, [r1, #-4]
    str r0, [sp, #0x1c]
    b L_744
L_6cc:
    ldr r0, [r9]
    orr r8, r8, #0x2000
    add r0, r0, #4
    str r0, [r9]
    ldr r1, [r9]
    ldr r0, [sp, #0xb0]
    ldr r6, [r1, #-4]
    str r0, [sp, #0x1c]
    b L_744
L_6f0:
    ldr r0, [r9]
    orr r8, r8, #0x2000
    add r0, r0, #4
    str r0, [r9]
    ldr r0, [sp, #0xc4]
    ldr r1, [r9]
    str r0, [sp, #0x20]
    ldr r0, [sp, #0xc8]
    ldr r6, [r1, #-4]
    str r0, [sp, #0x1c]
    b L_744
L_71c:
    ldr r0, [r9]
    orr r8, r8, #0x2000
    add r0, r0, #4
    str r0, [r9]
    ldr r0, [sp, #0xc8]
    ldr r1, [r9]
    str r0, [sp, #0x1c]
    ldr r0, [sp, #0xa0]
    ldr r6, [r1, #-4]
    str r0, [sp, #0x20]
L_744:
    ldr r0, [sp, #0x28]
    add r4, sp, #0x100
    cmp r0, #0x3c
    mov r5, r0
    mvn r0, #0
    ldrgt r5, [sp, #0xcc]
    cmp r7, r0
    add r4, r4, #0x9b
    moveq r7, r10
    beq L_7d8
    cmp r7, #0x3a
    ldrgt r7, [sp, #0xd0]
    b L_7d8
L_778:
    ldr r1, [sp, #0x1c]
    mov r0, r6
    bl func_0206c93c
    ldr r1, [sp, #0x1c]
    mul r1, r0, r1
    sub r1, r6, r1
    add r2, r1, #0x30
    cmp r2, #0x39
    ldrhi r1, [sp, #0x20]
    mov r6, r0
    addhi r2, r2, r1
    ands r0, r4, #1
    ldreqh r1, [r4]
    andeq r0, r2, #0xff
    andeq r1, r1, #0xff00
    orreq r0, r1, r0
    streqh r0, [r4]
    beq L_7d4
    ldrh r1, [r4, #-1]
    and r0, r2, #0xff
    and r1, r1, #0xff
    orr r0, r1, r0, lsl #8
    strh r0, [r4, #-1]
L_7d4:
    sub r4, r4, #1
L_7d8:
    cmp r7, #0
    sub r7, r7, #1
    bgt L_778
    cmp r6, #0
    bne L_778
    ldr r0, [sp, #0x1c]
    ldr r1, [sp, #0xd4]
    cmp r0, #0xa
    beq L_87c
    ands r0, r8, #0x10
    beq L_8b4
    ldr r0, [sp, #0xc]
    cmp r0, #6
    bne L_828
    ldr r0, [sp, #0xd8]
    ldr r1, [sp, #0xe0]
    str r0, [sp, #0x60]
    ldr r0, [sp, #0xdc]
    str r0, [sp, #0x64]
    b L_8b4
L_828:
    cmp r0, #7
    bne L_848
    ldr r0, [sp, #0xd8]
    ldr r1, [sp, #0xe8]
    str r0, [sp, #0x60]
    ldr r0, [sp, #0xe4]
    str r0, [sp, #0x64]
    b L_8b4
L_848:
    cmp r0, #5
    ldreq r0, [sp, #0xd8]
    moveq r1, r10
    streq r0, [sp, #0x60]
    beq L_8b4
    cmp r0, #4
    bne L_8b4
    ldr r0, [sp, #0xd8]
    ldr r1, [sp, #0xf0]
    str r0, [sp, #0x60]
    ldr r0, [sp, #0xec]
    str r0, [sp, #0x64]
    b L_8b4
L_87c:
    ands r0, r8, #0x4000
    ldrne r0, [sp, #0xf4]
    movne r1, r10
    strne r0, [sp, #0x60]
    bne L_8b4
    ands r0, r8, #2
    ldrne r0, [sp, #0xf8]
    movne r1, r10
    strne r0, [sp, #0x60]
    bne L_8b4
    ands r0, r8, #8
    ldrne r0, [sp, #0xfc]
    movne r1, r10
    strne r0, [sp, #0x60]
L_8b4:
    add r0, sp, #0x100
    add r0, r0, #0x9b
    sub r0, r0, r4
    sub r0, r0, r1
    sub r5, r5, r0
    cmp r5, #0
    ble L_91c
    ands r0, r8, #4
    beq L_91c
    cmp r5, #0
    ldr r0, [sp, #0x100]
    ble L_91c
    b L_914
L_8e8:
    ands r2, r4, #1
    ldrneh r2, [r4, #-1]
    add r0, r0, #1
    andne r2, r2, #0xff
    orrne r2, r2, #0x3000
    strneh r2, [r4, #-1]
    ldreqh r2, [r4]
    andeq r2, r2, #0xff00
    orreq r2, r2, #0x30
    streqh r2, [r4]
    sub r4, r4, #1
L_914:
    cmp r0, r5
    blt L_8e8
L_91c:
    cmp r1, #0
    beq L_9ac
    cmp r1, #2
    bne L_96c
    ands r0, r4, #1
    beq L_950
    ldrh r1, [r4, #-1]
    ldr r0, [sp, #0x64]
    and r0, r0, #0xff
    and r1, r1, #0xff
    orr r0, r1, r0, lsl #8
    strh r0, [r4, #-1]
    b L_968
L_950:
    ldrh r1, [r4]
    ldr r0, [sp, #0x64]
    and r0, r0, #0xff
    and r1, r1, #0xff00
    orr r0, r1, r0
    strh r0, [r4]
L_968:
    sub r4, r4, #1
L_96c:
    ands r0, r4, #1
    beq L_990
    ldrh r1, [r4, #-1]
    ldr r0, [sp, #0x60]
    and r0, r0, #0xff
    and r1, r1, #0xff
    orr r0, r1, r0, lsl #8
    strh r0, [r4, #-1]
    b L_9a8
L_990:
    ldrh r1, [r4]
    ldr r0, [sp, #0x60]
    and r0, r0, #0xff
    and r1, r1, #0xff00
    orr r0, r1, r0
    strh r0, [r4]
L_9a8:
    sub r4, r4, #1
L_9ac:
    add r0, sp, #0x100
    add r0, r0, #0x9b
    cmp r5, #0
    sub r6, r0, r4
    ble L_9d4
    ands r0, r8, #5
    bne L_9d4
    mov r0, r11
    mov r1, r5
    bl func_0206c9f4
L_9d4:
    add r1, r4, #1
    mov r2, r6
    mov r0, r11
    ldr r3, [r11, #0x10]
    blx r3
    cmp r5, #0
    ble L_132c
    ands r0, r8, #1
    beq L_132c
    mov r1, r5
    mov r0, r11
    bl func_0206c9f4
    b L_132c
L_a08:
    ldr r0, [r9]
    add r6, sp, #0x100
    add r0, r0, #8
    str r0, [r9]
    ldr r2, [r9]
    ldr r0, [sp, #0x28]
    ldr r1, [r2, #-8]
    str r0, [sp, #0x38]
    ldr r0, [r2, #-4]
    ldr r5, [sp, #0x104]
    str r0, [sp, #0x158]
    mov r0, r5
    str r0, [sp, #0x40]
    str r0, [sp, #0x58]
    add r0, sp, #0x154
    str r1, [sp, #0x154]
    add r1, r0, #7
    ands r0, r1, #1
    ldrneh r0, [r1, #-1]
    add r6, r6, #0x9b
    str r10, [sp, #0x5c]
    andne r0, r0, #0xff00
    movne r0, r0, asr #8
    ldreqh r0, [r1]
    orr r8, r8, #0x2000
    andeq r0, r0, #0xff
    ands r0, r0, #0x80
    beq L_acc
    add r0, sp, #0x154
    add r1, r0, #7
    ands r0, r1, #1
    ldrneh r0, [r1, #-1]
    orr r8, r8, #0x4000
    andne r0, r0, #0xff00
    movne r0, r0, asr #8
    ldreqh r0, [r1]
    andeq r0, r0, #0xff
    and r1, r0, #0x7f
    add r0, sp, #0x154
    add r0, r0, #7
    ands r2, r0, #1
    ldrneh r2, [r0, #-1]
    andne r2, r2, #0xff
    orrne r1, r2, r1, lsl #8
    strneh r1, [r0, #-1]
    ldreqh r2, [r0]
    andeq r2, r2, #0xff00
    orreq r1, r2, r1
    streqh r1, [r0]
L_acc:
    ldr r0, [sp, #0x28]
    cmp r0, #0x3c
    ldr r0, [sp, #0x154]
    str r0, [sp, #0x2c]
    ldr r0, [sp, #0x158]
    str r0, [sp, #0x18]
    ldrgt r0, [sp, #0xcc]
    ldr r1, [sp, #0x18]
    strgt r0, [sp, #0x38]
    ldr r0, [sp, #0x2c]
    bl func_0206c8b4
    ldr r1, [pc, #0x790]
    cmp r0, r1
    bge L_b40
    ldr r4, [sp, #0x108]
    b L_b24
L_b0c:
    ldr r0, [sp, #0x2c]
    ldr r1, [sp, #0x18]
    bl func_0206c244
    str r0, [sp, #0x2c]
    str r1, [sp, #0x18]
    sub r4, r4, #1
L_b24:
    ldr r0, [sp, #0x2c]
    ldr r1, [sp, #0x18]
    bl func_0206c8b4
    ldr r1, [pc, #0x758]
    cmp r0, r1
    blt L_b0c
    b L_bac
L_b40:
    ldr r0, [sp, #0x2c]
    ldr r1, [sp, #0x18]
    ldr r2, [sp, #0x10c]
    bl func_0206bdb4
    bl func_0206c8b4
    ldr r1, [pc, #0x734]
    cmp r0, r1
    blt L_ba8
    ldr r4, [sp, #0x110]
    b L_b84
L_b68:
    ldr r0, [sp, #0x2c]
    ldr r1, [sp, #0x18]
    mov r2, r10
    bl func_0206bdb4
    str r0, [sp, #0x2c]
    str r1, [sp, #0x18]
    add r4, r4, #1
L_b84:
    ldr r0, [sp, #0x2c]
    ldr r1, [sp, #0x18]
    ldr r2, [sp, #0x114]
    bl func_0206bdb4
    bl func_0206c8b4
    ldr r1, [pc, #0x6f0]
    cmp r0, r1
    bge L_b68
    b L_bac
L_ba8:
    ldr r4, [sp, #0x118]
L_bac:
    ldr r0, [sp, #0xc]
    cmp r0, #0xa
    bne L_bc8
    rsb r0, r7, #0x3a
    cmp r4, r0
    ldrgt r0, [sp, #0xb0]
    strgt r0, [sp, #0xc]
L_bc8:
    ldr r0, [sp, #0xc]
    sub r1, r0, #0xb
    cmp r1, #1
    ldrhi r0, [sp, #0x11c]
    strls r10, [sp, #0x10]
    strhi r0, [sp, #0x10]
    mvn r0, #0
    cmp r7, r0
    ldreq r7, [sp, #0x9c]
    cmp r7, #0x3a
    ldrgt r7, [sp, #0xd0]
    cmp r1, #1
    bhi L_c34
    mvn r0, #3
    cmp r4, r0
    blt L_c10
    cmp r4, r7
    blt L_c2c
L_c10:
    ldr r0, [sp, #0xc]
    cmp r0, #0xb
    ldreq r0, [sp, #0xb0]
    streq r0, [sp, #0xc]
    ldrne r0, [sp, #0xb4]
    strne r0, [sp, #0xc]
    b L_c34
L_c2c:
    ldr r0, [sp, #0xac]
    str r0, [sp, #0xc]
L_c34:
    ldr r0, [sp, #0x10]
    str r7, [sp, #0x3c]
    cmp r0, #1
    bne L_c68
    ldr r0, [sp, #0xc]
    cmp r7, #0
    streq r10, [sp, #0x3c]
    cmp r0, #0xa
    addeq r1, r4, #1
    ldr r0, [sp, #0x3c]
    movne r1, r10
    sub r0, r0, r1
    str r0, [sp, #0x3c]
L_c68:
    ldr r0, [sp, #0xc]
    ldr r2, [sp, #0x120]
    sub r0, r0, #8
    str r0, [sp, #0x6c]
    cmp r0, #1
    ldrls r0, [sp, #0x2c]
    ldr r3, [pc, #0x60c]
    strls r0, [sp, #0x154]
    ldrls r0, [sp, #0x18]
    strls r0, [sp, #0x158]
    mov r0, r2
    str r0, [sp, #0x44]
    b L_cc0
L_c9c:
    mov r0, r2
    mov r1, r3
    mov r2, r10
    bl func_0206bdb4
    mov r2, r0
    ldr r0, [sp, #0x44]
    mov r3, r1
    add r0, r0, #1
    str r0, [sp, #0x44]
L_cc0:
    ldr r1, [sp, #0x44]
    ldr r0, [sp, #0x3c]
    cmp r1, r0
    ble L_c9c
    ldr r0, [sp, #0x154]
    ldr r1, [sp, #0x158]
    bl func_0206c51c
    str r0, [sp, #0x154]
    ldr r0, [sp, #0x6c]
    str r1, [sp, #0x158]
    cmp r0, #1
    bhi L_e3c
    ldr r0, [sp, #0x10]
    cmp r0, #0
    beq L_d04
    cmp r4, #0
    beq L_e3c
L_d04:
    cmp r4, #0
    mvnlt r0, #0
    mullt r0, r4, r0
    movlt r4, r0
    ldrlt r0, [sp, #0x124]
    strge r10, [sp, #0x34]
    strlt r0, [sp, #0x34]
    ldr r0, [sp, #0x128]
    str r0, [sp, #0x70]
L_d28:
    ldr r1, [sp, #0xac]
    mov r0, r4
    bl func_0206c93c
    mov r1, #0xa
    mul r1, r0, r1
    sub r0, r4, r1
    add r0, r0, #0x30
    str r0, [sp, #0x48]
    ldr r1, [sp, #0xac]
    mov r0, r4
    bl func_0206c93c
    mov r4, r0
    ands r0, r6, #1
    beq L_d7c
    ldrh r1, [r6, #-1]
    ldr r0, [sp, #0x48]
    and r0, r0, #0xff
    and r1, r1, #0xff
    orr r0, r1, r0, lsl #8
    strh r0, [r6, #-1]
    b L_d94
L_d7c:
    ldrh r1, [r6]
    ldr r0, [sp, #0x48]
    and r0, r0, #0xff
    and r1, r1, #0xff00
    orr r0, r1, r0
    strh r0, [r6]
L_d94:
    ldr r0, [sp, #0x70]
    sub r6, r6, #1
    add r0, r0, #1
    add r5, r5, #1
    str r0, [sp, #0x70]
    cmp r0, #3
    blt L_d28
    ldr r0, [sp, #0x34]
    cmp r0, #0
    ldrne r1, [sp, #0xf8]
    ldreq r1, [sp, #0xf4]
    ands r0, r6, #1
    andeq r0, r1, #0xff
    ldreqh r1, [r6]
    andeq r1, r1, #0xff00
    orreq r0, r1, r0
    streqh r0, [r6]
    beq L_df0
    and r0, r1, #0xff
    ldrh r1, [r6, #-1]
    and r1, r1, #0xff
    orr r0, r1, r0, lsl #8
    strh r0, [r6, #-1]
L_df0:
    ldr r0, [sp, #0xc]
    cmp r0, #8
    ldreq r2, [sp, #0x12c]
    sub r0, r6, #1
    ldrne r2, [sp, #0x130]
    ands r1, r0, #1
    andeq r1, r2, #0xff
    ldreqh r2, [r0]
    andeq r2, r2, #0xff00
    orreq r1, r2, r1
    streqh r1, [r0]
    beq L_e34
    and r1, r2, #0xff
    ldrh r2, [r0, #-1]
    and r2, r2, #0xff
    orr r1, r2, r1, lsl #8
    strh r1, [r0, #-1]
L_e34:
    sub r6, r6, #2
    add r5, r5, #2
L_e3c:
    ldr r0, [sp, #0x154]
    str r0, [sp, #0x4c]
    ldr r0, [sp, #0x158]
    str r0, [sp, #8]
    ldr r0, [sp, #0x134]
    str r0, [sp, #0x54]
    b L_f10
L_e58:
    ldr r0, [sp, #0x4c]
    ldr r1, [sp, #8]
    bl func_0206c244
    str r0, [sp, #0x4c]
    str r1, [sp, #8]
    bl func_0206bc8c
    add r0, r0, #0x30
    ldr r1, [sp, #0x54]
    add r2, sp, #0x19c
    add r1, r2, r1
    ands r2, r1, #1
    ldreqh r2, [r1]
    and r0, r0, #0xff
    andeq r0, r0, #0xff
    andeq r2, r2, #0xff00
    orreq r0, r2, r0
    streqh r0, [r1]
    beq L_eb4
    ldrh r2, [r1, #-1]
    and r0, r0, #0xff
    and r2, r2, #0xff
    orr r0, r2, r0, lsl #8
    strh r0, [r1, #-1]
L_eb4:
    ands r0, r1, #1
    ldrneh r0, [r1, #-1]
    andne r0, r0, #0xff00
    movne r0, r0, asr #8
    ldreqh r0, [r1]
    andeq r0, r0, #0xff
    cmp r0, #0x30
    bge L_ef8
    ands r0, r1, #1
    ldrneh r0, [r1, #-1]
    andne r0, r0, #0xff
    orrne r0, r0, #0x3000
    strneh r0, [r1, #-1]
    ldreqh r0, [r1]
    andeq r0, r0, #0xff00
    orreq r0, r0, #0x30
    streqh r0, [r1]
L_ef8:
    ldr r0, [sp, #0x54]
    add r0, r0, #1
    str r0, [sp, #0x54]
    ldr r0, [sp, #0x3c]
    sub r0, r0, #1
    str r0, [sp, #0x3c]
L_f10:
    ldr r0, [sp, #0x3c]
    cmp r0, #0
    bgt L_e58
    b L_fd4
L_f20:
    ldr r0, [sp, #0x10]
    cmp r0, #1
    ldr r0, [sp, #0x54]
    sub r0, r0, #1
    str r0, [sp, #0x54]
    bne L_f70
    ldr r0, [sp, #0x5c]
    cmp r0, #0
    beq L_f70
    ldr r0, [sp, #0x54]
    add r1, sp, #0x19c
    add r1, r1, r0
    ands r0, r1, #1
    ldrneh r0, [r1, #-1]
    andne r0, r0, #0xff00
    movne r0, r0, asr #8
    ldreqh r0, [r1]
    andeq r0, r0, #0xff
    cmp r0, #0x30
    beq L_fd4
L_f70:
    ldr r0, [sp, #0x138]
    add r1, sp, #0x19c
    str r0, [sp, #0x5c]
    ldr r0, [sp, #0x54]
    add r1, r1, r0
    ands r0, r1, #1
    ldrneh r0, [r1, #-1]
    andne r0, r0, #0xff00
    movne r1, r0, asr #8
    ldreqh r0, [r1]
    andeq r1, r0, #0xff
    ands r0, r6, #1
    andeq r0, r1, #0xff
    ldreqh r1, [r6]
    andeq r1, r1, #0xff00
    orreq r0, r1, r0
    streqh r0, [r6]
    beq L_fcc
    and r0, r1, #0xff
    ldrh r1, [r6, #-1]
    and r1, r1, #0xff
    orr r0, r1, r0, lsl #8
    strh r0, [r6, #-1]
L_fcc:
    sub r6, r6, #1
    add r5, r5, #1
L_fd4:
    ldr r0, [sp, #0x54]
    cmp r0, #0
    bgt L_f20
    ldr r0, [sp, #0x5c]
    cmp r0, #0
    beq L_ff4
    ands r0, r8, #0x10
    beq L_1020
L_ff4:
    ands r0, r6, #1
    ldrneh r0, [r6, #-1]
    add r5, r5, #1
    andne r0, r0, #0xff
    orrne r0, r0, #0x2e00
    strneh r0, [r6, #-1]
    ldreqh r0, [r6]
    andeq r0, r0, #0xff00
    orreq r0, r0, #0x2e
    streqh r0, [r6]
    sub r6, r6, #1
L_1020:
    rsb r0, r7, #0x3a
    str r0, [sp, #0x68]
    b L_1054
L_102c:
    ldr r0, [sp, #0x58]
    ldr r1, [sp, #0x158]
    add r0, r0, #1
    str r0, [sp, #0x58]
    ldr r0, [sp, #0x154]
    mov r2, r10
    bl func_0206bdb4
    str r0, [sp, #0x154]
    str r1, [sp, #0x158]
    sub r4, r4, #1
L_1054:
    ldr r0, [sp, #0x68]
    cmp r4, r0
    bgt L_102c
    ldr r0, [sp, #0x154]
    ldr r1, [sp, #0x158]
    bl func_0206bc8c
    add r0, r0, #0x30
    and r4, r0, #0xff
    cmp r4, #0x30
    ldrlt r4, [sp, #0xd8]
    ldr r0, [sp, #0x154]
    ldr r1, [sp, #0x158]
    ldr r2, [sp, #0x13c]
    bl func_0206bdb4
    str r0, [sp, #0x154]
    str r1, [sp, #0x158]
    and r1, r4, #0xff
    ands r0, r6, #1
    andeq r0, r1, #0xff
    ldreqh r1, [r6]
    andeq r1, r1, #0xff00
    orreq r0, r1, r0
    streqh r0, [r6]
    beq L_10c8
    and r0, r1, #0xff
    ldrh r1, [r6, #-1]
    and r1, r1, #0xff
    orr r0, r1, r0, lsl #8
    strh r0, [r6, #-1]
L_10c8:
    sub r6, r6, #1
    add r5, r5, #1
    b L_1144
L_10d4:
    ldr r0, [sp, #0x154]
    ldr r1, [sp, #0x158]
    bl func_0206bc8c
    add r0, r0, #0x30
    and r4, r0, #0xff
    cmp r4, #0x30
    ldrlt r4, [sp, #0xd8]
    ldr r0, [sp, #0x154]
    ldr r1, [sp, #0x158]
    ldr r2, [sp, #0x140]
    bl func_0206bdb4
    str r0, [sp, #0x154]
    str r1, [sp, #0x158]
    and r1, r4, #0xff
    ands r0, r6, #1
    andeq r0, r1, #0xff
    ldreqh r1, [r6]
    andeq r1, r1, #0xff00
    orreq r0, r1, r0
    streqh r0, [r6]
    beq L_113c
    and r0, r1, #0xff
    ldrh r1, [r6, #-1]
    and r1, r1, #0xff
    orr r0, r1, r0, lsl #8
    strh r0, [r6, #-1]
L_113c:
    sub r6, r6, #1
    add r5, r5, #1
L_1144:
    ldr r0, [sp, #0x154]
    ldr r1, [sp, #0x158]
    bl func_0206c8b4
    ldr r1, [pc, #0x138]
    cmp r0, r1
    bge L_10d4
    ands r0, r8, #0x4000
    ldr r1, [sp, #0x144]
    ldrne r0, [sp, #0xf4]
    movne r1, r10
    strne r0, [sp, #0x40]
    addne r5, r5, #1
    bne L_11a4
    ands r0, r8, #2
    ldrne r0, [sp, #0xf8]
    movne r1, r10
    strne r0, [sp, #0x40]
    addne r5, r5, #1
    bne L_11a4
    ands r0, r8, #8
    ldrne r0, [sp, #0xfc]
    movne r1, r10
    strne r0, [sp, #0x40]
    addne r5, r5, #1
L_11a4:
    ldr r0, [sp, #0x38]
    sub r0, r0, r5
    str r0, [sp, #0x38]
    cmp r0, #0
    ble L_120c
    ands r0, r8, #4
    beq L_120c
    ldr r0, [sp, #0x38]
    cmp r0, #0
    ldr r0, [sp, #0x148]
    ble L_120c
    b L_1200
L_11d4:
    ands r2, r6, #1
    ldrneh r2, [r6, #-1]
    add r0, r0, #1
    andne r2, r2, #0xff
    orrne r2, r2, #0x3000
    strneh r2, [r6, #-1]
    ldreqh r2, [r6]
    andeq r2, r2, #0xff00
    orreq r2, r2, #0x30
    streqh r2, [r6]
    sub r6, r6, #1
L_1200:
    ldr r2, [sp, #0x38]
    cmp r0, r2
    blt L_11d4
L_120c:
    cmp r1, #0
    beq L_1254
    ands r0, r6, #1
    beq L_1238
    ldrh r1, [r6, #-1]
    ldr r0, [sp, #0x40]
    and r0, r0, #0xff
    and r1, r1, #0xff
    orr r0, r1, r0, lsl #8
    strh r0, [r6, #-1]
    b L_1250
L_1238:
    ldrh r1, [r6]
    ldr r0, [sp, #0x40]
    and r0, r0, #0xff
    and r1, r1, #0xff00
    orr r0, r1, r0
    strh r0, [r6]
L_1250:
    sub r6, r6, #1
L_1254:
    ldr r0, [sp, #0x38]
    cmp r0, #0
    add r0, sp, #0x100
    add r0, r0, #0x9b
    sub r4, r0, r6
    ble L_1304
    ands r0, r8, #5
    bne L_1304
    ldr r1, [sp, #0x38]
    mov r0, r11
    bl func_0206c9f4
    b L_1304
    dcd data_0209a0a0
    dcd data_0209a130
    dcd data_02086894
    dcd 0x000003ff
    dcd 0x40140000
L_1298:
    add r6, r6, #1
    ands r0, r6, #1
    ldrneh r0, [r6, #-1]
    andne r0, r0, #0xff00
    movne r0, r0, asr #8
    ldreqh r0, [r6]
    andeq r0, r0, #0xff
    cmp r0, #0x2e
    bne L_12ec
    b L_12e0
L_12c0:
    ldr r1, [pc, #0x98]
    ldr r3, [r11, #0x10]
    mov r0, r11
    mov r2, r10
    blx r3
    ldr r0, [sp, #0x58]
    sub r0, r0, #1
    str r0, [sp, #0x58]
L_12e0:
    ldr r0, [sp, #0x58]
    cmp r0, #0
    bgt L_12c0
L_12ec:
    mov r0, r11
    mov r1, r6
    mov r2, r10
    ldr r3, [r11, #0x10]
    blx r3
    sub r4, r4, #1
L_1304:
    cmp r4, #0
    bgt L_1298
    ldr r0, [sp, #0x38]
    cmp r0, #0
    ble L_132c
    ands r0, r8, #1
    beq L_132c
    ldr r1, [sp, #0x38]
    mov r0, r11
    bl func_0206c9f4
L_132c:
    ldr r4, [sp, #0x14c]
    b L_190
L_1334:
    ldr r0, [sp, #0x24]
    cmp r0, #0
    beq L_1354
    ldr r3, [r11, #0x10]
    ldr r2, [sp, #0x24]
    add r1, sp, #0x15c
    mov r0, r11
    blx r3
L_1354:
    add sp, sp, #0x1dc
    ldmia sp!, {r4-r11, lr}
    bx lr
}
