typedef int s32;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef unsigned char u8;
typedef long long s64;
typedef long long ret2_t;

extern ret2_t func_0206bc8c(s32, s32);
extern ret2_t func_0206bdb4(s32, s32, s32);
extern ret2_t func_0206c244(s32, s32);
extern ret2_t func_0206c51c(s32, s32);
extern s32 func_0206c8b4(s32, s32);
extern s32 func_0206c93c(s32, u32);
extern void func_0206c9f4(int *fp, s32);
extern s32 func_0206ca44(s32 *);

extern s32 data_02086894;
extern s32 data_0208689c;
extern u8 data_0209a0a0;
extern u8 data_0209a130;

#define FP_WRITE(fp, buf, n) ((void (*)(int *, int *, int))((fp)[4]))((fp), (int *)(buf), (n))

void func_0206a928(register int *fp_in, unsigned fmt, int *args) {
    register int *sb;
    register u8 var_r1;
    register s32 var_r8;
    register u32 var_r7;
    register u32 var_r4;
    register s32 var_r10;
    register int *file;
    s32 sp0;
    s32 sp4;
    s32 sp8;
    u32 spC;
    s32 sp10;
    s32 sp14;
    char sp18_hole[4];
    u32 sp1C;
    u32 sp20;
    s32 sp24;
    s32 sp28;
    s32 sp2C;
    s32 sp30;
    s32 sp34;
    s32 sp38;
    s32 sp3C;
    s32 sp40;
    s32 sp44;
    s32 sp48;
    s32 sp4C;
    s32 sp50;
    s32 sp54;
    s32 sp58;
    s32 sp5C;
    s32 sp60;
    s32 sp64;
    s32 sp68;
    u32 sp6C;
    s32 sp70;
    s32 sp74;
    s32 sp78;
    u32 sp7C;
    s32 sp80;
    s32 sp84;
    u32 sp88;
    u32 sp8C;
    u32 sp90;
    u32 sp94;
    u32 sp98;
    u32 sp9C;
    u32 spA0;
    u32 spA4;
    u32 spA8;
    u32 spAC;
    u32 spB0;
    u32 spB4;
    u32 spB8;
    u32 spBC;
    u32 spC0;
    u32 spC4;
    u32 spC8;
    s32 spCC;
    u32 spD0;
    u32 spD4;
    s32 spD8;
    s32 spDC;
    u32 spE0;
    s32 spE4;
    u32 spE8;
    s32 spEC;
    u32 spF0;
    s32 spF4;
    s32 spF8;
    s32 spFC;
    s32 sp100;
    s32 sp104;
    s32 sp108;
    s32 sp10C;
    s32 sp110;
    s32 sp114;
    s32 sp118;
    s32 sp11C;
    s32 sp120;
    s32 sp124;
    s32 sp128;
    s32 sp12C;
    s32 sp130;
    s32 sp134;
    s32 sp138;
    s32 sp13C;
    s32 sp140;
    u32 sp144;
    s32 sp148;
    u32 sp14C;
    s32 sp150;
    s32 sp154;
    s32 sp158;
    volatile char sp15C[0x40];
    volatile char sp19C[0x40];
    volatile char sp19C_pad[0x18];
    volatile char frame_pad[8];
    s32 temp_gt;
    s32 temp_r0_11;
    void *temp_r0_12;
    s32 temp_r0_14;
    s32 temp_r0_2;
    s32 temp_r0_5;
    s32 temp_r0_6;
    s32 temp_r0_8;
    s32 temp_r1_13;
    s32 temp_r1_14;
    s32 temp_r1_15;
    s32 temp_r1_3;
    s32 temp_r1_4;
    s32 temp_r1_5;
    s32 temp_r1_7;
    s32 temp_r1_8;
    s32 temp_r1_9;
    s32 temp_r4;
    s32 temp_r5;
    s32 temp_r6;
    s32 temp_ret;
    s32 temp_ret_2;
    s32 temp_ret_3;
    s32 temp_ret_4;
    s32 temp_ret_5;
    s32 temp_ret_6;
    s32 temp_ret_7;
    s32 temp_ret_8;
    s32 temp_z;
    s32 var_r0;
    s32 var_r0_2;
    s32 var_r0_3;
    s32 var_r0_4;
    s32 var_r0_5;
    s32 var_r0_6;
    s32 var_r0_7;
    s32 var_r0_8;
    s32 var_r1_3;
    s32 var_r1_4;
    s32 var_r1_5;
    s32 var_r2_2;
    s32 var_r2_3;
    s32 var_r3;
    s32 var_r4_3;
    s32 var_r4_6;
    s32 var_r5;
    s32 var_r5_2;
    s32 var_r5_3;
    s32 var_r5_4;
    s32 var_r6;
    void *var_r6_3;
    u16 temp_r0_3;
    u32 temp_r0_10;
    u32 temp_r0_7;
    u32 temp_r1_6;
    u32 var_r1_2;
    u32 var_r1_6;
    u32 var_r2;
    u8 temp_r0_13;
    u8 var_r4_4;
    u8 var_r4_5;
    void *temp_r0;
    void *temp_r0_4;
    void *temp_r0_9;
    void *temp_r1;
    void *temp_r1_10;
    void *temp_r1_11;
    void *temp_r1_12;
    void *temp_r1_2;
    void *temp_r2;
    void *var_r4_2;
    void *var_r6_2;

    asm {
        mov r11, r0
        mov r0, #0x2
        str r0, [sp, #0xc]
        mov r0, #0xa
        str r0, [sp, #0xac]
        mov r0, #0x10
        str r0, [sp, #0xc8]
        mov r0, #0x7
        str r0, [sp, #0xa0]
        mov r0, #0x27
        str r0, [sp, #0xc4]
        mov r0, #0x8
        str r0, [sp, #0xb0]
        ldr r0, [sp, #0xc]
        mov r8, #0x0
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
        mvn r3, #0x0
        str r0, [sp, #0xf0]
        mov r0, #0x58
        str r0, [sp, #0xe4]
        ldr r0, [sp, #0xc]
        str r3, [sp, #0x28]
        str r0, [sp, #0xe8]
        mov r0, #0x78
        str r0, [sp, #0xdc]
        ldr r0, [sp, #0xc]
        str r1, [sp, #0x0]
        str r0, [sp, #0xe0]
        mov r0, #0x6
        str r0, [sp, #0x9c]
        mov r0, #0x9
        str r0, [sp, #0xb4]
        mov r0, #0x45
        str r0, [sp, #0x130]
        mov r0, #0x65
        str r0, [sp, #0x12c]
        ldr r0, [sp, #0xc]
        mov r9, r2
        str r0, [sp, #0x8c]
        mov r0, #0x3
        str r0, [sp, #0x90]
        mov r0, #0x5
        str r0, [sp, #0x98]
        mov r0, #0xc
        str r0, [sp, #0xbc]
        mov r0, #0xb
        str r0, [sp, #0xb8]
        ldr r0, [sp, #0xc]
        str r8, [sp, #0x20]
        str r0, [sp, #0xa8]
        mov r0, #0x4
        str r0, [sp, #0x94]
        mov r0, r3
        mov r7, r3
        mov r4, r8
        str r8, [sp, #0x24]
        mov r10, #0x1
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
    }
loop_1:
    asm {
        ldr r0, [sp, #0x0]
        ands r0, r0, #0x1
        ldreq r0, [sp, #0x0]
        ldreqh r0, [r0, #0x0]
        andeq r1, r0, #0xff
        ldrne r0, [sp, #0x0]
        ldrneh r0, [r0, #-0x1]
        andne r0, r0, #0xff00
        movne r1, r0, asr #0x8
        cmp r1, #0x0
        beq loop_1_zero
    }
        sp0 += 1;
        if (var_r4 == 0) {
            if (var_r1 == 0x25) {
                if (sp24 != 0) {
                    FP_WRITE(file, &sp15C, sp24);
                    sp24 = sp74;
                }
                var_r8 = sp78;
                var_r4 = var_r10;
                sp28 = var_r8;
                var_r7 = sp7C;
            } else {
                temp_r0 = (void *)((char *)&sp15C + sp24);
                if (!((s32)(u32)temp_r0 & 1)) {
                    *(u16 *)(temp_r0) = (u16) ((*(u16 *)(temp_r0) & 0xFF00) | var_r1);
                } else {
                    *(u16 *)((char *)(temp_r0) - 2) = (u16) ((u8) *(u16 *)((char *)(temp_r0) - 2) | (var_r1 << 8));
                }
                temp_r0_2 = sp24 + 1;
                sp24 = temp_r0_2;
                if (temp_r0_2 == 0x40) {
                    FP_WRITE(file, &sp15C, sp24);
                    sp24 = sp80;
                }
            }
        } else {
            temp_r0_3 = *(u8 *)((char *)&data_0209a0a0 + (var_r4 - 1) * 0x12 + *(u8 *)((char *)&data_0209a130 + var_r1 * 2) * 2);
            switch ((u8) temp_r0_3) {               /* switch 1 */
            default:                                /* switch 1 */
                break;
            case 1:                                 /* switch 1 */
                temp_r0_4 = (void *)((char *)&sp15C + sp24);
                if (!((s32) temp_r0_4 & 1)) {
                    *(u16 *)(temp_r0_4) = (u16) ((*(u16 *)(temp_r0_4) & 0xFF00) | var_r1);
                } else {
                    *(u16 *)((char *)(temp_r0_4) - 2) = (u16) ((u8) *(u16 *)((char *)(temp_r0_4) - 2) | (var_r1 << 8));
                }
                temp_r0_5 = sp24 + 1;
                sp24 = temp_r0_5;
                if (temp_r0_5 == 0x40) {
                    FP_WRITE(file, &sp15C, sp24);
                    sp24 = sp84;
                }
                var_r4 = sp88;
                break;
            case 2:                                 /* switch 1 */
                var_r4 = sp8C;
                switch ((s32) var_r1) {             /* switch 2; irregular */
                case 35:                            /* switch 2 */
                    var_r8 |= 0x10;
                    break;
                case 48:                            /* switch 2 */
                    var_r8 |= 4;
                    break;
                case 43:                            /* switch 2 */
                    var_r8 |= 2;
                    break;
                case 45:                            /* switch 2 */
                    var_r8 |= 1;
                    break;
                case 32:                            /* switch 2 */
                    var_r8 |= 8;
                    break;
                }
                break;
            case 3:                                 /* switch 1 */
                if (sp28 == -1) {
                    var_r0 = var_r1 - 0x30;
                    sp28 = var_r0;
                } else {
                    var_r0 = (0xA * sp28) + (var_r1 - 0x30);
                }
                var_r4 = sp90;
                if (sp28 != -1) {
                    sp28 = var_r0;
                }
                break;
            case 4:                                 /* switch 1 */
                var_r4 = sp94;
                break;
            case 5:                                 /* switch 1 */
                if (var_r7 == -1) {
                    var_r7 = var_r1 - 0x30;
                } else {
                    var_r7 = (0xA * var_r7) + (var_r1 - 0x30);
                }
                var_r4 = sp98;
                break;
            case 6:                                 /* switch 1 */
                var_r4 = sp9C;
                break;
            case 7:                                 /* switch 1 */
                var_r4 = spA0;
                if ((s32) var_r1 <= 0x58) {
                    switch ((s32) var_r1) {         /* switch 3; irregular */
                    case 0x47:                      /* switch 3 */
                        spC = spBC;
                        break;
                    default:                        /* switch 3 */
                        spC = var_r4;
                        break;
                    case 0x45:                      /* switch 3 */
                        spC = spB4;
                        break;
                    }
                } else if ((s32) var_r1 <= 0x69) {
                    temp_r0_6 = var_r1 - 0x62;
                    switch (temp_r0_6) {            /* switch 4; irregular */
                    case 0:                         /* switch 4 */
                        spC = sp94;
                        break;
                    case 1:                         /* switch 4 */
                        spC = 1;
                        break;
                    case 2:                         /* switch 4 */
                    case 7:                         /* switch 4 */
                        spC = spA8;
                        break;
                    case 4:                         /* switch 4 */
                        spC = spAC;
                        break;
                    case 3:                         /* switch 4 */
                        spC = spB0;
                        break;
                    case 5:                         /* switch 4 */
                        spC = spB8;
                        break;
                    }
                } else {
                    temp_r0_7 = var_r1 - 0x6F;
                    switch (temp_r0_7) {            /* switch 5 */
                    default:                        /* switch 5 */
                        break;
                    case 4:                         /* switch 5 */
                        spC = spA4;
                        break;
                    case 6:                         /* switch 5 */
                        spC = sp90;
                        break;
                    case 0:                         /* switch 5 */
                        spC = sp98;
                        break;
                    case 9:                         /* switch 5 */
                        spC = sp9C;
                        break;
                    }
                }
                break;
            }
            if (var_r4 == 7) {
                switch (spC) {                      /* switch 6 */
                default:                            /* switch 6 */
                    break;
                case 1:                             /* switch 6 */
                    sb = (int *)((char *)sb + 4);
                    sp150 = *(int *)((char *)sb - 4);
                    FP_WRITE(file, &sp150, 1);
                    break;
                case 0:                             /* switch 6 */
                    sb = (int *)((char *)sb + 4);
                    var_r5 = *(int *)((char *)sb - 4);
                    if (var_r5 == 0) {
                        var_r5 = (s32)(int)&data_02086894;
                    }
                    temp_r4 = func_0206ca44((s32 *) var_r5);
                    temp_r6 = var_r8 & 1;
                    if ((temp_r6 == 0) && (temp_r4 < sp28)) {
                        func_0206c9f4(file, sp28 - temp_r4);
                    }
                    FP_WRITE(file, (s32 *) var_r5, temp_r4);
                    if ((temp_r6 != 0) && (temp_r4 < sp28)) {
                        func_0206c9f4(file, sp28 - temp_r4);
                    }
                    break;
                case 2:                             /* switch 6 */
                    sb = (int *)((char *)sb + 4);
                    var_r6 = *(int *)((char *)sb - 4);
                    if (var_r6 < 0) {
                        var_r8 |= 0x4000;
                        var_r6 = 0 - var_r6;
                    }
                    var_r8 |= 0x1000;
                    sp1C = spAC;
block_140:
                    var_r5_2 = sp28;
                    if (sp28 > 0x3C) {
                        var_r5_2 = spCC;
                    }
                    var_r4_2 = (void *)((char *)&sp100 + 0x9B);
                    if (var_r7 == -1) {
                        var_r7 = 1;
                    } else if ((s32) var_r7 > 0x3A) {
                        var_r7 = spD0;
                    }
loop_153:
                    temp_gt = (s32) var_r7 > 0;
                    var_r7 -= 1;
                    if (temp_gt) {
block_147:
                        temp_r0_8 = func_0206c93c(var_r6, sp1C);
                        var_r2 = (var_r6 - (temp_r0_8 * sp1C)) + 0x30;
                        var_r6 = temp_r0_8;
                        if (var_r2 > 0x39U) {
                            var_r2 += sp20;
                        }
                        if (!((s32)(u32)var_r4_2 & 1)) {
                            *(u16 *)(var_r4_2) = (u16) ((*(u16 *)(var_r4_2) & 0xFF00) | (u8) var_r2);
                        } else {
                            *(u16 *)((char *)(var_r4_2) - 2) = (u16) ((u8) *(u16 *)((char *)(var_r4_2) - 2) | ((u8) var_r2 << 8));
                        }
                        var_r4_2 = (void *)((char *)var_r4_2 - 1);
                        goto loop_153;
                    }
                    if (var_r6 != 0) {
                        goto block_147;
                    }
                    var_r1_2 = spD4;
                    if (sp1C != 0xA) {
                        if (var_r8 & 0x10) {
                            switch (spC) {          /* switch 7; irregular */
                            case 6:                 /* switch 7 */
                                var_r1_2 = spE0;
                                sp60 = spD8;
                                sp64 = spDC;
                                break;
                            case 7:                 /* switch 7 */
                                var_r1_2 = spE8;
                                sp60 = spD8;
                                sp64 = spE4;
                                break;
                            case 5:                 /* switch 7 */
                                var_r1_2 = 1;
                                sp60 = spD8;
                                break;
                            case 4:                 /* switch 7 */
                                var_r1_2 = spF0;
                                sp60 = spD8;
                                sp64 = spEC;
                                break;
                            }
                        }
                    } else if (var_r8 & 0x4000) {
                        var_r1_2 = 1;
                        sp60 = spF4;
                    } else if (var_r8 & 2) {
                        var_r1_2 = 1;
                        sp60 = spF8;
                    } else if (var_r8 & 8) {
                        var_r1_2 = 1;
                        sp60 = spFC;
                    }
                    temp_r5 = var_r5_2 - ((s32)((char *)&sp100 + 0x9B - (char *)var_r4_2) - var_r1_2);
                    if ((temp_r5 > 0) && (var_r8 & 4)) {
                        var_r0_2 = sp100;
                        if (temp_r5 > 0) {
loop_179:
                            if (var_r0_2 < temp_r5) {
                                var_r0_2 += 1;
                                if ((s32) var_r4_2 & 1) {
                                    *(u16 *)((char *)(var_r4_2) - 2) = (u16) ((u8) *(u16 *)((char *)(var_r4_2) - 2) | 0x3000);
                                } else {
                                    *(u16 *)(var_r4_2) = (u16) ((*(u16 *)(var_r4_2) & 0xFF00) | 0x30);
                                }
                                var_r4_2 = (void *)((char *)var_r4_2 - 1);
                                goto loop_179;
                            }
                        }
                    }
                    if (var_r1_2 != 0) {
                        if (var_r1_2 == 2) {
                            if ((s32) var_r4_2 & 1) {
                                *(u16 *)((char *)(var_r4_2) - 2) = (u16) ((u8) *(u16 *)((char *)(var_r4_2) - 2) | ((u8) sp64 << 8));
                            } else {
                                *(u16 *)(var_r4_2) = (u16) ((*(u16 *)(var_r4_2) & 0xFF00) | (u8) sp64);
                            }
                            var_r4_2 = (void *)((char *)var_r4_2 - 1);
                        }
                        if ((s32) var_r4_2 & 1) {
                            *(u16 *)((char *)(var_r4_2) - 2) = (u16) ((u8) *(u16 *)((char *)(var_r4_2) - 2) | ((u8) sp60 << 8));
                        } else {
                            *(u16 *)(var_r4_2) = (u16) ((*(u16 *)(var_r4_2) & 0xFF00) | (u8) sp60);
                        }
                        var_r4_2 = (void *)((char *)var_r4_2 - 1);
                    }
                    if ((temp_r5 > 0) && !(var_r8 & 5)) {
                        func_0206c9f4(file, temp_r5);
                    }
                    FP_WRITE(file, var_r4_2 + 1, ((void *)((char *)&sp100 + 0x9B)) - var_r4_2);
                    if ((temp_r5 > 0) && (var_r8 & 1)) {
                        func_0206c9f4(file, temp_r5);
                    }
                    break;
                case 3:                             /* switch 6 */
                    var_r8 |= 0x2000;
                    sb = (int *)((char *)sb + 4);
                    var_r6 = *(int *)((char *)sb - 4);
                    sp1C = spAC;
                    goto block_140;
                case 4:                             /* switch 6 */
                    var_r8 |= 0x2000;
                    sb = (int *)((char *)sb + 4);
                    var_r6 = *(int *)((char *)sb - 4);
                    sp1C = spC0;
                    goto block_140;
                case 5:                             /* switch 6 */
                    var_r8 |= 0x2000;
                    sb = (int *)((char *)sb + 4);
                    var_r6 = *(int *)((char *)sb - 4);
                    sp1C = spB0;
                    goto block_140;
                case 6:                             /* switch 6 */
                    var_r8 |= 0x2000;
                    sb = (int *)((char *)sb + 4);
                    sp20 = spC4;
                    var_r6 = *(int *)((char *)sb - 4);
                    sp1C = spC8;
                    goto block_140;
                case 7:                             /* switch 6 */
                    var_r8 |= 0x2000;
                    sb = (int *)((char *)sb + 4);
                    sp1C = spC8;
                    var_r6 = *(int *)((char *)sb - 4);
                    sp20 = spA0;
                    goto block_140;
                case 8:                             /* switch 6 */
                case 9:                             /* switch 6 */
                case 10:                            /* switch 6 */
                case 11:                            /* switch 6 */
                case 12:                            /* switch 6 */
                    sb = (int *)((char *)sb + 8);
                    temp_r2 = (void *)sb;
                    sp38 = sp28;
                    var_r5_3 = sp104;
                    sp158 = *(int *)((char *)temp_r2 - 4);
                    sp40 = var_r5_3;
                    sp58 = var_r5_3;
                    sp154 = *(int *)((char *)temp_r2 - 8);
                    temp_r1 = (void *)((char *)&sp154 + 7);
                    var_r0_3 = (s32) temp_r1 & 1;
                    temp_z = var_r0_3 == 0;
                    var_r6_2 = (void *)((char *)&sp100 + 0x9B);
                    if (var_r0_3 != 0) {
                        var_r0_3 = (s32) (*(u16 *)((char *)(temp_r1) - 2) & 0xFF00) >> 8;
                    }
                    sp5C = 1;
                    var_r8 |= 0x2000;
                    if (temp_z) {
                        var_r0_3 = (s32) (u8) *(u16 *)((char *)(void *)((char *)&sp154 + 7));
                    }
                    if (var_r0_3 & 0x80) {
                        temp_r1_2 = (void *)((char *)&sp154 + 7);
                        var_r8 |= 0x4000;
                        if ((s32) temp_r1_2 & 1) {
                            var_r0_4 = (s32) (*(u16 *)((char *)(temp_r1_2) - 2) & 0xFF00) >> 8;
                        } else {
                            var_r0_4 = (s32) (u8) *(u16 *)((char *)(void *)((char *)&sp154 + 7));
                        }
                        temp_r1_3 = var_r0_4 & 0x7F;
                        temp_r0_9 = (void *)((char *)&sp154 + 7);
                        if ((s32) temp_r0_9 & 1) {
                            *(u16 *)((char *)(temp_r0_9) - 2) = (u16) ((u8) *(u16 *)((char *)(temp_r0_9) - 2) | (temp_r1_3 << 8));
                        } else {
                            *(u16 *)((char *)(void *)((char *)&sp154 + 7)) = (u16) ((*(u16 *)((char *)(void *)((char *)&sp154 + 7)) & 0xFF00) | temp_r1_3);
                        }
                    }
                    sp2C = sp154;
                    *(s32 *)sp18_hole = sp158;
                    if (sp28 > 0x3C) {
                        sp38 = spCC;
                    }
                    if (func_0206c8b4(sp2C, *(s32 *)sp18_hole) < 0x3FF) {
                        var_r4_3 = sp108;
loop_212:
                        if (func_0206c8b4(sp2C, *(s32 *)sp18_hole) < 0x3FF) {
                            { ret2_t _dr = func_0206c244(sp2C, *(s32 *)sp18_hole);
                              sp2C = (s32)_dr;
                              *(s32 *)sp18_hole = (s32)((u32)((u64)_dr >> 32)); }
                            var_r4_3 -= 1;
                            goto loop_212;
                        }
                    } else {
                        func_0206bdb4(sp2C, *(s32 *)sp18_hole, sp10C);
                        if (func_0206c8b4(sp2C, *(s32 *)sp18_hole) >= 0x3FF) {
                            var_r4_3 = sp110;
loop_217:
                            func_0206bdb4(sp2C, *(s32 *)sp18_hole, sp114);
                            if (func_0206c8b4(sp2C, *(s32 *)sp18_hole) >= 0x3FF) {
                                { ret2_t _dr = func_0206bdb4(sp2C, *(s32 *)sp18_hole, 1);
                                  sp2C = (s32)_dr;
                                  *(s32 *)sp18_hole = (s32)((u32)((u64)_dr >> 32)); }
                                var_r4_3 += 1;
                                goto loop_217;
                            }
                        } else {
                            var_r4_3 = sp118;
                        }
                    }
                    if ((spC == 0xA) && (var_r4_3 > (s32) (0x3A - var_r7))) {
                        spC = spB0;
                    }
                    temp_r1_6 = spC - 0xB;
                    if (temp_r1_6 > 1) {
                        sp10 = sp11C;
                    } else {
                        sp10 = 1;
                    }
                    if (var_r7 == -1) {
                        var_r7 = sp9C;
                    }
                    if ((s32) var_r7 > 0x3A) {
                        var_r7 = spD0;
                    }
                    if (temp_r1_6 <= 1) {
                        if ((var_r4_3 < -4) || (var_r4_3 >= (s32) var_r7)) {
                            if (spC == 0xB) {
                                spC = spB0;
                            } else {
                                spC = spB4;
                            }
                        } else {
                            spC = spAC;
                        }
                    }
                    sp3C = (s32) var_r7;
                    if (sp10 == 1) {
                        if (var_r7 == 0) {
                            sp3C = 1;
                        }
                        if (spC == 0xA) {
                            var_r1_3 = var_r4_3 + 1;
                        } else {
                            var_r1_3 = 1;
                        }
                        sp3C -= var_r1_3;
                    }
                    var_r2_2 = sp120;
                    temp_r0_10 = spC - 8;
                    sp6C = temp_r0_10;
                    var_r3 = 0x40140000;
                    if (temp_r0_10 <= 1) {
                        sp154 = sp2C;
                        sp158 = *(s32 *)sp18_hole;
                    }
                    sp44 = var_r2_2;
loop_249:
                    if (sp44 <= sp3C) {
                        { ret2_t _dr = func_0206bdb4(var_r2_2, var_r3, 1);
                          var_r2_2 = (s32)_dr;
                          var_r3 = (s32)((u32)((u64)_dr >> 32)); }
                        sp44 += 1;
                        goto loop_249;
                    }
                    { ret2_t _dr = func_0206c51c(sp154, sp158);
                      sp154 = (s32)_dr;
                      sp158 = (s32)((u32)((u64)_dr >> 32)); }
                    if ((sp6C <= 1) && ((sp10 == 0) || (var_r4_3 != 0))) {
                        if (var_r4_3 < 0) {
                            var_r4_3 *= -1;
                            sp34 = sp124;
                        } else {
                            sp34 = 1;
                        }
                        sp70 = sp128;
                        do {
                            sp48 = (var_r4_3 - (func_0206c93c(var_r4_3, spAC) * 0xA)) + 0x30;
                            var_r4_3 = func_0206c93c(var_r4_3, spAC);
                            if ((s32) var_r6_2 & 1) {
                                *(u16 *)((char *)(var_r6_2) - 2) = (u16) ((u8) *(u16 *)((char *)(var_r6_2) - 2) | ((u8) sp48 << 8));
                            } else {
                                *(u16 *)(var_r6_2) = (u16) ((*(u16 *)(var_r6_2) & 0xFF00) | (u8) sp48);
                            }
                            var_r6_2 = (void *)((char *)var_r6_2 - 1);
                            temp_r0_11 = sp70 + 1;
                            var_r5_3 += 1;
                            sp70 = temp_r0_11;
                        } while (temp_r0_11 < 3);
                        if (sp34 != 0) {
                            var_r1_4 = spF8;
                        } else {
                            var_r1_4 = spF4;
                        }
                        if (!((s32)(u32)var_r6_2 & 1)) {
                            *(u16 *)(var_r6_2) = (u16) ((*(u16 *)(var_r6_2) & 0xFF00) | (u8) var_r1_4);
                        } else {
                            *(u16 *)((char *)(var_r6_2) - 2) = (u16) ((u8) *(u16 *)((char *)(var_r6_2) - 2) | ((u8) var_r1_4 << 8));
                        }
                        temp_r0_12 = (void *)((char *)var_r6_2 - 1);
                        if (spC == 8) {
                            var_r2_3 = sp12C;
                        } else {
                            var_r2_3 = sp130;
                        }
                        if (!((s32)(u32)temp_r0_12 & 1)) {
                            *(u16 *)(temp_r0_12) = (u16) ((*(u16 *)(temp_r0_12) & 0xFF00) | (u8) var_r2_3);
                        } else {
                            *(u16 *)((char *)(temp_r0_12) - 2) = (u16) ((u8) *(u16 *)((char *)(temp_r0_12) - 2) | ((u8) var_r2_3 << 8));
                        }
                        var_r6_2 = (void *)((char *)var_r6_2 - 2);
                        var_r5_3 += 2;
                    }
                    sp4C = sp154;
                    sp8 = sp158;
                    sp54 = sp134;
loop_286:
                    if (sp3C > 0) {
                        { ret2_t _dr = func_0206c244(sp4C, sp8);
                          sp4C = (s32)_dr;
                          sp8 = (s32)((u32)((u64)_dr >> 32)); }
                        temp_r1_10 = (void *)((char *)&sp19C + sp54);
                        temp_r0_13 = func_0206bc8c(sp4C, sp8) + 0x30;
                        if (!((s32) temp_r1_10 & 1)) {
                            *(u16 *)(temp_r1_10) = (u16) ((*(u16 *)(temp_r1_10) & 0xFF00) | temp_r0_13);
                        } else {
                            *(u16 *)((char *)(temp_r1_10) - 2) = (u16) ((u8) *(u16 *)((char *)(temp_r1_10) - 2) | (temp_r0_13 << 8));
                        }
                        if ((s32) temp_r1_10 & 1) {
                            var_r0_5 = (s32) (*(u16 *)((char *)(temp_r1_10) - 2) & 0xFF00) >> 8;
                        } else {
                            var_r0_5 = (s32) (u8) *(u16 *)(temp_r1_10);
                        }
                        if (var_r0_5 < 0x30) {
                            if ((s32) temp_r1_10 & 1) {
                                *(u16 *)((char *)(temp_r1_10) - 2) = (u16) ((u8) *(u16 *)((char *)(temp_r1_10) - 2) | 0x3000);
                            } else {
                                *(u16 *)(temp_r1_10) = (u16) ((*(u16 *)(temp_r1_10) & 0xFF00) | 0x30);
                            }
                        }
                        sp54 += 1;
                        sp3C -= 1;
                        goto loop_286;
                    }
loop_301:
                    if (sp54 > 0) {
                        sp54 -= 1;
                        if ((sp10 == 1) && (sp5C != 0)) {
                            temp_r1_11 = (void *)((char *)&sp19C + sp54);
                            if ((s32) temp_r1_11 & 1) {
                                var_r0_6 = (s32) (*(u16 *)((char *)(temp_r1_11) - 2) & 0xFF00) >> 8;
                            } else {
                                var_r0_6 = (s32) (u8) *(u16 *)(temp_r1_11);
                            }
                            if (var_r0_6 != 0x30) {
                                goto block_294;
                            }
                        } else {
block_294:
                            sp5C = sp138;
                            temp_r1_12 = (void *)((char *)&sp19C + sp54);
                            if ((s32) temp_r1_12 & 1) {
                                var_r1_5 = (s32) (*(u16 *)((char *)(temp_r1_12) - 2) & 0xFF00) >> 8;
                            } else {
                                var_r1_5 = (s32) (u8) *(u16 *)(temp_r1_12);
                            }
                            if (!((s32)(u32)var_r6_2 & 1)) {
                                *(u16 *)(var_r6_2) = (u16) ((*(u16 *)(var_r6_2) & 0xFF00) | (u8) var_r1_5);
                            } else {
                                *(u16 *)((char *)(var_r6_2) - 2) = (u16) ((u8) *(u16 *)((char *)(var_r6_2) - 2) | ((u8) var_r1_5 << 8));
                            }
                            var_r6_2 = (void *)((char *)var_r6_2 - 1);
                            var_r5_3 += 1;
                        }
                        goto loop_301;
                    }
                    if ((sp5C == 0) || (var_r8 & 0x10)) {
                        var_r5_3 += 1;
                        if ((s32) var_r6_2 & 1) {
                            *(u16 *)((char *)(var_r6_2) - 2) = (u16) ((u8) *(u16 *)((char *)(var_r6_2) - 2) | 0x2E00);
                        } else {
                            *(u16 *)(var_r6_2) = (u16) ((*(u16 *)(var_r6_2) & 0xFF00) | 0x2E);
                        }
                        var_r6_2 = (void *)((char *)var_r6_2 - 1);
                    }
                    sp68 = 0x3A - var_r7;
loop_310:
                    if (var_r4_3 > sp68) {
                        sp58 += 1;
                        { ret2_t _dr = func_0206bdb4(sp154, sp158, 1);
                          sp154 = (s32)_dr;
                          sp158 = (s32)((u32)((u64)_dr >> 32)); }
                        var_r4_3 -= 1;
                        goto loop_310;
                    }
                    var_r4_4 = func_0206bc8c(sp154, sp158) + 0x30;
                    if ((s32) var_r4_4 < 0x30) {
                        var_r4_4 = (u8) spD8;
                    }
                    { ret2_t _dr = func_0206bdb4(sp154, sp158, sp13C);
                      sp154 = (s32)_dr;
                      sp158 = (s32)((u32)((u64)_dr >> 32)); }
                    if (!((s32)(u32)var_r6_2 & 1)) {
                        *(u16 *)(var_r6_2) = (u16) ((*(u16 *)(var_r6_2) & 0xFF00) | var_r4_4);
                    } else {
                        *(u16 *)((char *)(var_r6_2) - 2) = (u16) ((u8) *(u16 *)((char *)(var_r6_2) - 2) | (var_r4_4 << 8));
                    }
                    var_r6_3 = (void *)((char *)var_r6_2 - 1);
                    var_r5_4 = var_r5_3 + 1;
loop_323:
                    if (func_0206c8b4(sp154, sp158) >= 0x3FF) {
                        var_r4_5 = func_0206bc8c(sp154, sp158) + 0x30;
                        if ((s32) var_r4_5 < 0x30) {
                            var_r4_5 = (u8) spD8;
                        }
                        { ret2_t _dr = func_0206bdb4(sp154, sp158, sp140);
                          sp154 = (s32)_dr;
                          sp158 = (s32)((u32)((u64)_dr >> 32)); }
                        if (!((s32)(u32)var_r6_3 & 1)) {
                            *(u16 *)(var_r6_3) = (u16) ((*(u16 *)(var_r6_3) & 0xFF00) | var_r4_5);
                        } else {
                            *(u16 *)((char *)(var_r6_3) - 2) = (u16) ((u8) *(u16 *)((char *)(var_r6_3) - 2) | (var_r4_5 << 8));
                        }
                        var_r6_3 = (void *)((char *)var_r6_3 - 1);
                        var_r5_4 += 1;
                        goto loop_323;
                    }
                    var_r1_6 = sp144;
                    if (var_r8 & 0x4000) {
                        var_r1_6 = 1;
                        sp40 = spF4;
                        var_r5_4 += 1;
                    } else if (var_r8 & 2) {
                        var_r1_6 = 1;
                        sp40 = spF8;
                        var_r5_4 += 1;
                    } else if (var_r8 & 8) {
                        var_r1_6 = 1;
                        sp40 = spFC;
                        var_r5_4 += 1;
                    }
                    temp_r0_14 = sp38 - var_r5_4;
                    sp38 = temp_r0_14;
                    if ((temp_r0_14 > 0) && (var_r8 & 4)) {
                        var_r0_7 = sp148;
                        if (sp38 > 0) {
loop_338:
                            if (var_r0_7 < sp38) {
                                var_r0_7 += 1;
                                if ((s32)(u32)var_r6_3 & 1) {
                                    *(u16 *)((char *)(var_r6_3) - 2) = (u16) ((u8) *(u16 *)((char *)(var_r6_3) - 2) | 0x3000);
                                } else {
                                    *(u16 *)(var_r6_3) = (u16) ((*(u16 *)(var_r6_3) & 0xFF00) | 0x30);
                                }
                                var_r6_3 = (void *)((char *)var_r6_3 - 1);
                                goto loop_338;
                            }
                        }
                    }
                    if (var_r1_6 != 0) {
                        if ((s32)(u32)var_r6_3 & 1) {
                            *(u16 *)((char *)(var_r6_3) - 2) = (u16) ((u8) *(u16 *)((char *)(var_r6_3) - 2) | ((u8) sp40 << 8));
                        } else {
                            *(u16 *)(var_r6_3) = (u16) ((*(u16 *)(var_r6_3) & 0xFF00) | (u8) sp40);
                        }
                        var_r6_3 = (void *)((char *)var_r6_3 - 1);
                    }
                    var_r4_6 = (s32)((char *)&sp100 + 0x9B - (char *)var_r6_3);
                    if ((sp38 > 0) && !(var_r8 & 5)) {
                        func_0206c9f4(file, sp38);
                    }
loop_355:
                    if (var_r4_6 > 0) {
                        var_r6_3 = (void *)((char *)var_r6_3 + 1);
                        if ((s32)(u32)var_r6_3 & 1) {
                            var_r0_8 = (s32) (*(u16 *)((char *)(var_r6_3) - 2) & 0xFF00) >> 8;
                        } else {
                            var_r0_8 = (s32) (u8) *(u16 *)(var_r6_3);
                        }
                        if (var_r0_8 == 0x2E) {
loop_353:
                            if (sp58 > 0) {
                                FP_WRITE(file, &data_0208689c, 1);
                                sp58 -= 1;
                                goto loop_353;
                            }
                        }
                        FP_WRITE(file, (s32 *) var_r6_3, 1);
                        var_r4_6 -= 1;
                        goto loop_355;
                    }
                    if ((sp38 > 0) && (var_r8 & 1)) {
                        func_0206c9f4(file, sp38);
                    }
                    break;
                }
                var_r4 = sp14C;
            }
        }
        goto loop_1;
loop_1_zero:
    if (sp24 != 0) {
        FP_WRITE(file, &sp15C, sp24);
    }
    (void)sp19C_pad;

}