#include "types.h"
/* A calibrated touch-panel sample (nitro/spi/tp.h's TPData, as func_0203df40 uses;
   x, y, touch, validity). */
struct Col4 {
    u16 x, y, touch, validity;
};

/* The per-frame input record, named as func_0203df40's view: data_020a1040 is
   the local one, data_020a1154 the four players'. */
typedef struct {
    s32 frame;         /* 0x00 */
    struct Col4 touch; /* 0x04 */
    u16 flags;         /* 0x0c */
    u16 keys;          /* 0x0e */
    s16 inputYaw;      /* 0x10 */
    u8 settings[0x10]; /* 0x12 -- the link settings bytes (GetPlayerFlagByte, func_0203db3c) */
    u8 unk22;
    u8 unk23;
} Rec;

extern void func_020408b0(s32 a);
extern void CpuCopy8(void *dst, void *src, s32 len);
extern void func_0205a588(void *dst, s32 val, s32 len);
extern s32 func_02040714(void);
extern void func_02040820(void);
extern void func_02040790(void);
extern void func_02040c34(s32 a, s32 b, void *c, void *d, s32 e);
extern s32 func_020406b4(void *a, void *b);
extern u8 func_02040704(s32 a);
extern void *func_0204068c(u16 a);
extern s32 func_0204271c(void *o);
extern s32 func_02042748(void *a, void *b);
extern s32 func_02040a84(void);
extern void func_02040a5c(void);
extern s32 func_0203fd64(void);
extern void func_02042778(void);
extern void *func_0203f604(void);
extern void func_0203f644(void);

extern u8 data_02099e1c;
extern u8 data_02099e18;
extern u8 data_020a0ef0;
extern u16 data_020a0f1c;
extern u16 data_020a0f28;
extern u8 data_020a0ef8;
/* data_020a1020.. is the packet this console sends each frame, packed from
   the local record and unpacked the same way into data_020a1154 below: +0x0
   flags, +0x2 frame, +0x6 keys, +0x8/+0x9/+0xa touch x/y/touch (one byte
   each), +0xb inputYaw, +0xd data_020a0f08, +0xe data_020a0f00, +0xf the
   0x11 bytes at Rec 0x12. While data_020a0ef8 counts down it carries instead
   a chunk index at +0x2 and one of the 0x16-byte chunks at data_020a10a4 at
   +0x3. */
extern u16 data_020a1020;
extern u16 data_020a104c; /* data_020a1040.flags */
extern u8 data_020a1022;
extern u8 data_020a1023[0x16];
extern u16 data_020a1026;
extern u8 data_020a1028;
extern u8 data_020a1029;
extern u8 data_020a102a;
extern u16 data_020a102b;
extern u8 data_020a102d;
extern u8 data_020a102e;
extern u8 data_020a102f[0x11];
extern Rec data_020a1040;
extern u8 data_020a1044;  /* data_020a1040.touch.x */
extern u8 data_020a1046;  /* data_020a1040.touch.y */
extern u8 data_020a1048;  /* data_020a1040.touch.touch */
extern u16 data_020a104e; /* data_020a1040.keys */
extern u16 data_020a1050; /* data_020a1040.inputYaw */
extern u8 data_020a0f08;
extern u8 data_020a0f00;
extern u8 data_020a1052[0x11]; /* data_020a1040.settings */
extern u8 data_020a10a4[0x16];
extern u8 data_020a10ba[0x16];
extern u8 data_020a10d0[0x16];
extern u8 data_020a10e6[0x16];
extern u8 data_020a1112[0x16];
extern u8 data_020a0f04;
extern u16 data_020a1160; /* data_020a1154[0].flags */
extern Rec data_020a1154[4];
extern Rec data_020a1178; /* data_020a1154[1] */
extern u8 data_020a0f10;
extern u8 data_020a0fa6[24];
extern u8 data_020a0fbe[24];
extern s16 data_020a0f2c;
extern u16 data_020a0f30;
extern u8 data_020a0efc;

/* The link step for modes 1 and 2 (func_0203df40): pack the local input
   record into the send packet (see data_020a1020), hand it to the link, and
   unpack each player's received packet (func_0204068c, by player index) into
   data_020a1154. The copies take their sizes from sp14 (2: flags, keys,
   inputYaw), sp18 (4: frame), sp1c (0x11) and sp24 (0x24, a record). */
void func_0203ea5c(void) {
    s32 sp4;
    s32 sp8;
    s32 sp28;
    u8 *var_sb;
    s32 sp14;
    s32 sp18;
    s32 spC;
    u8 *var_r8;
    s32 temp_r0_4;
    s32 sp1c;
    s32 temp_r0_7;
    u16 sp2e;
    Rec *var_r3_3;
    s32 sp10;
    Rec *var_r6_4;
    u8 *var_r7_2;
    s32 sp20;
    Rec *var_r1;
    s32 temp_r0;
    u16 temp_r1;
    u32 temp_r0_2;
    s32 temp_r0_8;
    s32 var_r0;
    s32 var_r6_2;
    Rec *var_r5_4;
    u8 *var_r6_3;
    s32 var_r2_4;
    Rec *var_r4;
    s32 var_r3_2;
    s32 var_r4_2;
    s32 var_r5_2;
    s32 var_r5_3;
    Rec *cur133;
    s32 var_r2_3;
    s32 sp24;
    u16 temp_r1_2;
    u8 temp_r0_9;
    u16 temp_r2;
    s32 var_r7;
    s32 var_r3;
    u8 *temp_r0_6;
    u8 *temp_r5;
    s32 temp_r0_3;
    u8 sp2c;
    s32 var_r2_2;
    Rec *var_r6;
    s32 temp_r0_5;
    Rec *var_r5;
    u32 n75;
    s32 var_r2;
    u8 temp_r1_3;
    u16 temp_r1_4;

    /* var_r6_2, var_r3, var_r2_2 and var_r4_2 stay 0/1 search flags: an inline
       AllPeersHave()/AnyPeerHas() predicate re-colours registers (13-17 insns) */
    if (data_02099e1c != 0) {
        func_020408b0(2);
        data_02099e1c = 0;
    }
    sp8 = 0;
    if (data_020a0ef0 != 0) {
        sp4 = 0x12C;
    } else {
        sp4 = 0x4B0;
    }
    data_020a0f1c |= 0x2000;
    data_020a0f1c &= 0xF0FF;
    data_020a0f1c |= (data_020a0f28 << 8) & 0xF00;
    sp18 = 4;
    sp14 = 2;
    sp1c = 0x11;
    sp24 = 0x24;
    sp10 = 3;
    sp28 = 5;
    sp20 = 0x12C;
    while ((sp8 == 0) && (sp4 != 0)) {
        if (data_020a0ef8 != 0) {
            if (data_020a0f1c & 0x4000) {
                data_020a0ef8 -= 1;
            }
            if (data_020a0ef8 != 0) {
                if ((u32)data_020a0ef8 > 4U) {
                    sp2c = (u8)sp10;
                } else {
                    sp2c = data_020a0ef8 - 1;
                }
                data_020a1040.flags = (u16)(data_020a1040.flags | 1);
                CpuCopy8(&data_020a104c, &data_020a1020, sp14);
                CpuCopy8(&sp2c, &data_020a1022, 1);
                CpuCopy8((u8 *)((sp2c * 0x16) + (s32)&data_020a10a4), &data_020a1023, 0x16);
            } else {
                data_020a1040.flags = (u16)(data_020a1040.flags & 0xFFFE);
            }
        }
        if (data_020a0ef8 == 0) {
            CpuCopy8(&data_020a104c, &data_020a1020, sp14);
            CpuCopy8(&data_020a1040, &data_020a1022, sp18);
            CpuCopy8(&data_020a104e, &data_020a1026, sp14);
            CpuCopy8(&data_020a1044, &data_020a1028, 1);
            CpuCopy8(&data_020a1046, &data_020a1029, 1);
            CpuCopy8(&data_020a1048, &data_020a102a, 1);
            CpuCopy8(&data_020a1050, &data_020a102b, sp14);
            CpuCopy8(&data_020a0f08, &data_020a102d, 1);
            CpuCopy8(&data_020a0f00, &data_020a102e, 1);
            CpuCopy8(&data_020a1052, &data_020a102f, sp1c);
        }
        data_020a0f1c &= 0xBFFF;
        temp_r0_2 = func_02040714();
        switch (temp_r0_2) {
        case 0:
            if (data_020a0f04 == 1) {
                func_02040820();
                func_02040c34(1, 1, &func_0203f644, &func_0203f604, 0);
            } else if (data_020a0f04 == 2) {
                func_02040790();
                func_02040c34(0, 1, &func_0203f644, &func_0203f604, 0);
            }
            sp8 = 0;
            break;
        case 1:
            sp8 = 0;
            break;
        case 2:
            sp8 = 0;
            break;
        case 3:
            temp_r0_3 = func_020406b4(&data_020a1020, &sp2e);
            sp8 = temp_r0_3;
            if (temp_r0_3 != 0) {
                data_020a0f1c |= 0xC000;
            }
            if (data_020a1154[0].flags & 0x2000) {
                sp8 = 1;
            }
            break;
        case 4:
            if ((data_020a1040.flags & 0x4000) && (data_020a1154[0].flags & 0x2000)) {
                sp8 = 1;
            } else {
                temp_r0_4 = func_020406b4(&data_020a1020, &sp2e);
                sp8 = temp_r0_4;
                if (temp_r0_4 != 0) {
                    data_020a0f1c |= 0xC000;
                }
                if (data_020a1040.flags & 0x4000) {
                    sp8 = 1;
                }
            }
            break;
        default:
            sp8 = 0;
            break;
        }
        temp_r0_5 = data_020a0f1c & 0x4000;
        if (temp_r0_5 != 0) {
            data_020a0f10 = func_02040704(temp_r0_5);
            data_02099e18 = 0;
            sp4 = sp20;
            temp_r0_6 = func_0204068c(0U);
            if (temp_r0_6 != 0) {
                CpuCopy8(temp_r0_6, &data_020a1160, sp14);
                if (data_020a1154[0].flags & 0x8000) {
                    if (data_020a1154[0].flags & 1) {
                        if (data_020a0f10 != 0) {
                            CpuCopy8(temp_r0_6 + 2, &sp2c, 1);
                            switch (sp2c) {
                            default:
                                break;
                            case 0:
                                data_020a1040.flags = (u16)(data_020a1040.flags & 0xFFFE);
                                func_0205a588(&data_020a10a4, 0, 0x16);
                                CpuCopy8(temp_r0_6 + 3, &data_020a10a4, 0x16);
                                break;
                            case 1:
                                func_0205a588(&data_020a10ba, 0, 0x16);
                                CpuCopy8(temp_r0_6 + 3, &data_020a10ba, 0x16);
                                break;
                            case 2:
                                func_0205a588(&data_020a10d0, 0, 0x16);
                                CpuCopy8(temp_r0_6 + 3, &data_020a10d0, 0x16);
                                break;
                            case 3:
                                func_0205a588(&data_020a10e6, 0, 0x16);
                                CpuCopy8(temp_r0_6 + 3, &data_020a10e6, 0x16);
                                break;
                            }
                        }
                        sp8 = 0;
                    } else {
                        var_r6 = data_020a1154;
                        var_r7 = 0;
                        do {
                            temp_r5 = func_0204068c(var_r7);
                            func_0205a588(var_r6, 0, sp24);
                            if (temp_r5 != 0) {
                                CpuCopy8(temp_r5, &var_r6->flags, sp14);
                                if (var_r6->flags & 0x8000) {
                                    data_02099e18 += 1;
                                    CpuCopy8(temp_r5 + 2, var_r6, sp18);
                                    CpuCopy8(temp_r5 + 6, &var_r6->keys, sp14);
                                    CpuCopy8(temp_r5 + 8, &var_r6->touch.x, 1);
                                    CpuCopy8(temp_r5 + 9, &var_r6->touch.y, 1);
                                    CpuCopy8(temp_r5 + 0xA, &var_r6->touch.touch, 1);
                                    CpuCopy8(temp_r5 + 0xB, &var_r6->inputYaw, sp14);
                                    CpuCopy8(temp_r5 + 0xF, var_r6->settings, sp1c);
                                    if ((data_020a0f10 != 0) && (var_r7 == 0)) {
                                        CpuCopy8(temp_r5 + 0xD, &data_020a0f08, 1);
                                        CpuCopy8(temp_r5 + 0xE, &data_020a0f00, 1);
                                    }
                                }
                            }
                            var_r6 += 1;
                            var_r7 += 1;
                        } while ((s32)var_r7 < 4);
                        if (data_020a1154[data_020a0f10].flags & 0x8000) {
                            if (data_020a0f10 == 0) {
                                if (data_020a0ef8 == 0) {
                                    n75 = data_02099e18;
                                    if (n75 > 1U) {
                                        var_r5 = &data_020a1178;
                                        var_r6_2 = 1;
                                        var_r2 = 1;
                                        do {
                                            temp_r1 = var_r5->flags;
                                            if ((temp_r1 & 0x8000) && !(temp_r1 & 1)) {
                                                var_r6_2 = 0;
                                                break;
                                            }
                                            var_r5 += 1;
                                            var_r2 += 1;
                                        } while (var_r2 < 4);
                                        if ((var_r6_2 != 0) && (n75 > 1U)) {
                                            spC = 1;
                                            data_020a0ef8 = (u8)sp28;
                                            var_sb = data_020a0fa6;
                                            var_r8 = data_020a10ba;
                                            do {
                                                if (func_0204271c(var_sb) == 0) {
                                                    var_r7_2 = data_020a0fbe;
                                                    var_r6_3 = data_020a1112;
                                                    var_r5_2 = 1;
                                                    do {
                                                        if (func_02042748(var_sb, var_r7_2) != 0) {
                                                            CpuCopy8(var_r6_3, var_r8, 0x16);
                                                        }
                                                        var_r7_2 += 6;
                                                        var_r6_3 += 0x16;
                                                        var_r5_2 += 1;
                                                    } while (var_r5_2 < 4);
                                                }
                                                var_sb += 6;
                                                var_r8 += 0x16;
                                                spC = spC + 1;
                                            } while (spC < 4);
                                        }
                                    }
                                }
                                if ((u32)data_02099e18 > 1U) {
                                    var_r6_4 = &data_020a1178;
                                    var_r3 = 1;
                                    var_r5_3 = 1;
                                    do {
                                        temp_r2 = var_r6_4->flags;
                                        if ((temp_r2 & 0x8000) && !(temp_r2 & 0x4000)) {
                                            var_r3 = 0;
                                            break;
                                        }
                                        var_r6_4 += 1;
                                        var_r5_3 += 1;
                                    } while (var_r5_3 < 4);
                                    if ((var_r3 != 0) && !(data_020a1154[0].flags & 0x2000) &&
                                        !(data_020a1040.flags & 0x2000)) {
                                        if ((u32)data_02099e18 > 2U) {
                                            func_02040a84();
                                            func_02040a5c();
                                        }
                                        data_020a0f2c = 0;
                                        do {
                                        } while (func_0203fd64() != 1);
                                        data_020a1040.flags = (u16)(data_020a1040.flags | 0x2000);
                                    }
                                    var_r5_4 = &data_020a1178;
                                    var_r2_2 = 0;
                                    var_r3_2 = 1;
                                    do {
                                        temp_r1_2 = var_r5_4->flags;
                                        if ((temp_r1_2 & 0x8000) && (temp_r1_2 & 0x4000)) {
                                            var_r2_2 = 1;
                                            break;
                                        }
                                        var_r5_4 += 1;
                                        var_r3_2 += 1;
                                    } while (var_r3_2 < 4);
                                    if ((var_r2_2 == 0) && (data_020a1154[0].flags & 0x2000) &&
                                        (data_020a1040.flags & 0x2000)) {
                                        data_020a0f2c = 1;
                                        do {
                                        } while (func_0203fd64() != 1);
                                        data_020a1040.flags = (u16)(data_020a1040.flags & 0xDFFF);
                                    }
                                }
                            }
                            var_r0 = 0;
                            var_r1 = data_020a1154;
                            do {
                                if ((var_r1->flags & 0x8000) &&
                                    (var_r1->frame != data_020a1154[data_020a0f10].frame)) {
                                    data_020a1040.frame = data_020a1154[0].frame;
                                    data_020a0f1c |= 2;
                                    if (data_020a0efc == 0) {
                                        sp8 = 0;
                                    }
                                    break;
                                }
                                var_r1 += 1;
                                var_r0 += 1;
                            } while (var_r0 < 4);
                        } else {
                            sp8 = 0;
                        }
                    }
                } else {
                    sp8 = 0;
                }
            } else {
                sp8 = 0;
            }
        }
        if (sp4 != 0) {
            sp4 -= 1;
        }
        if (sp8 == 0) {
            func_02042778();
            temp_r0_8 = data_020a0f1c & 0x4000;
            if (temp_r0_8 != 0) {
                func_02042778();
            }
        }
    }
    data_020a0f1c &= 0xDFFF;
    if (sp4 != 0) {
        var_r4 = data_020a1154;
        var_r2_3 = 0;
        cur133 = &data_020a1154[data_020a0f10];
        do {
            if (var_r4->flags & 0x8000) {
                temp_r1_3 = cur133->unk22;
                temp_r0_9 = var_r4->unk22;
                if (temp_r0_9 != temp_r1_3) {
                    data_020a0f1c |= 8;
                    break;
                } else if (temp_r0_9 != temp_r1_3) {
                    data_020a0f1c |= 0x10;
                    break;
                }
            }
            var_r2_3 += 1;
            var_r4 += 1;
        } while (var_r2_3 < 4);
        if ((data_020a1154[0].flags & 0x2000) || (data_020a1040.flags & 0x4000)) {
            data_020a0efc = 3;
        }
        if (data_020a0efc != 0) {
            data_020a0f1c &= 0xFF60;
            data_020a1040.unk22 = 0;
            data_020a1040.unk22 = 0;
            data_020a0f30 = 0;
            data_020a1040.flags = (u16)(data_020a1040.flags & 0xEFFF);
            data_020a0efc -= 1;
        }
        if (data_020a0efc != 0) {
            return;
        }
        var_r3_3 = data_020a1154;
        var_r4_2 = 1;
        var_r2_4 = 0;
        do {
            temp_r1_4 = var_r3_3->flags;
            if ((temp_r1_4 & 0x8000) && !(temp_r1_4 & 0x1000)) {
                var_r4_2 = 0;
                break;
            }
            var_r2_4 += 1;
            var_r3_3 += 1;
        } while (var_r2_4 < 4);
        temp_r0 = data_020a1040.flags & 0x1000;
        if (temp_r0 == 0) {
            return;
        }
        if (var_r4_2 == 0) {
            return;
        }
        data_020a1040.flags = (u16)(data_020a1040.flags & 0xEFFF);
        data_020a0f30 = 1;
        return;
    }
    data_020a0f04 = 0;
    data_020a0f1c |= 1;
}
