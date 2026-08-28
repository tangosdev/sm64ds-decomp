#include "types.h"
typedef struct {
    s32 unk0;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u16 unkC;
    u16 unkE;
    u16 unk10;
    u8 unk12[0x10];
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
/* DEFECT D1, fixed here: the declaration follows the definition.
   src/func_02040a5c.c:11 is `void func_02040a5c(u32 val)` -- one argument,
   IRQ-guarded, and it stores `val & ~1u`. This TU used to declare it (void)
   and call it with nothing at the pair below, which is correct only on ARM,
   where func_02040a84's return is still sitting in r0 when the second bl
   lands. On a stack ABI it reads an unwritten slot. See the pair at the
   call site for the ROM-faithful shape. */
extern void func_02040a5c(u32 val);
extern s32 func_0203fd64(void);
extern void func_02042778(void);
/* DEFECT D3, fixed here, and it is the one nobody had named: this TU declared
   func_0203f604 as `void (void)` against src/func_0203f604.c:15's
   `void *func_0203f604(int unused, unsigned int size, void *ptr)` -- wrong
   return type AND three missing parameters. It matters more than the other two
   because this symbol is ADDRESS-TAKEN (passed to func_02040c34 at :194 and
   :197), so no call-site edit can reach it; the declaration is the only place
   the type exists. func_0203f604 is a combined allocate/free helper, so a call
   through a mis-typed pointer reaches Memory::Allocate with a garbage size. */
extern void *func_0203f604(int unused, unsigned int size, void *ptr);
extern void func_0203f644(void);

extern u8 data_02099e1c;
extern u8 data_02099e18;
extern u8 data_020a0ef0;
extern u16 data_020a0f1c;
extern u16 data_020a0f28;
extern u8 data_020a0ef8;
extern u16 data_020a1020;
extern u16 data_020a104c;
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
extern u8 data_020a1044;
extern u8 data_020a1046;
extern u8 data_020a1048;
extern u16 data_020a104e;
extern u16 data_020a1050;
extern u8 data_020a0f08;
extern u8 data_020a0f00;
extern u8 data_020a1052[0x11];
extern u8 data_020a10a4[0x16];
extern u8 data_020a10ba[0x16];
extern u8 data_020a10d0[0x16];
extern u8 data_020a10e6[0x16];
extern u8 data_020a1112[0x16];
extern u8 data_020a0f04;
extern u16 data_020a1160;
extern Rec data_020a1154[4];
extern Rec data_020a1178;
extern u8 data_020a0f10;
extern u8 data_020a0fa6[24];
extern u8 data_020a0fbe[24];
extern s16 data_020a0f2c;
extern u16 data_020a0f30;
extern u8 data_020a0efc;

void func_0203ea5c(void)
{
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
                if ((u32) data_020a0ef8 > 4U) {
                    sp2c = (u8) sp10;
                } else {
                    sp2c = data_020a0ef8 - 1;
                }
                data_020a1040.unkC = (u16) (data_020a1040.unkC | 1);
                CpuCopy8(&data_020a104c, &data_020a1020, sp14);
                CpuCopy8(&sp2c, &data_020a1022, 1);
                CpuCopy8((u8 *) ((sp2c * 0x16) + (s32) &data_020a10a4), &data_020a1023, 0x16);
            } else {
                data_020a1040.unkC = (u16) (data_020a1040.unkC & 0xFFFE);
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
            if (data_020a1154[0].unkC & 0x2000) {
                sp8 = 1;
            }
            break;
        case 4:
            if ((data_020a1040.unkC & 0x4000) && (data_020a1154[0].unkC & 0x2000)) {
                sp8 = 1;
            } else {
                temp_r0_4 = func_020406b4(&data_020a1020, &sp2e);
                sp8 = temp_r0_4;
                if (temp_r0_4 != 0) {
                    data_020a0f1c |= 0xC000;
                }
                if (data_020a1040.unkC & 0x4000) {
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
                if (data_020a1154[0].unkC & 0x8000) {
                    if (data_020a1154[0].unkC & 1) {
                        if (data_020a0f10 != 0) {
                            CpuCopy8(temp_r0_6 + 2, &sp2c, 1);
                            switch (sp2c) {
                            default:
                                break;
                            case 0:
                                data_020a1040.unkC = (u16) (data_020a1040.unkC & 0xFFFE);
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
                                CpuCopy8(temp_r5, &var_r6->unkC, sp14);
                                if (var_r6->unkC & 0x8000) {
                                    data_02099e18 += 1;
                                    CpuCopy8(temp_r5 + 2, var_r6, sp18);
                                    CpuCopy8(temp_r5 + 6, &var_r6->unkE, sp14);
                                    CpuCopy8(temp_r5 + 8, &var_r6->unk4, 1);
                                    CpuCopy8(temp_r5 + 9, &var_r6->unk6, 1);
                                    CpuCopy8(temp_r5 + 0xA, &var_r6->unk8, 1);
                                    CpuCopy8(temp_r5 + 0xB, &var_r6->unk10, sp14);
                                    CpuCopy8(temp_r5 + 0xF, var_r6->unk12, sp1c);
                                    if ((data_020a0f10 != 0) && (var_r7 == 0)) {
                                        CpuCopy8(temp_r5 + 0xD, &data_020a0f08, 1);
                                        CpuCopy8(temp_r5 + 0xE, &data_020a0f00, 1);
                                    }
                                }
                            }
                            var_r6 += 1;
                            var_r7 += 1;
                        } while ((s32) var_r7 < 4);
                        if (data_020a1154[data_020a0f10].unkC & 0x8000) {
                            if (data_020a0f10 == 0) {
                                if (data_020a0ef8 == 0) {
                                    n75 = data_02099e18;
                                    if (n75 > 1U) {
                                    var_r5 = &data_020a1178;
                                    var_r6_2 = 1;
                                    var_r2 = 1;
loop_75:
                                    temp_r1 = var_r5->unkC;
                                    if ((temp_r1 & 0x8000) && !(temp_r1 & 1)) {
                                        var_r6_2 = 0;
                                    } else {
                                        var_r5 += 1;
                                        var_r2 += 1;
                                        if (var_r2 < 4) {
                                            goto loop_75;
                                        }
                                    }
                                    if ((var_r6_2 != 0) && (n75 > 1U)) {
                                        spC = 1;
                                        data_020a0ef8 = (u8) sp28;
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
                                if ((u32) data_02099e18 > 1U) {
                                    var_r6_4 = &data_020a1178;
                                    var_r3 = 1;
                                    var_r5_3 = 1;
loop_90:
                                    temp_r2 = var_r6_4->unkC;
                                    if ((temp_r2 & 0x8000) && !(temp_r2 & 0x4000)) {
                                        var_r3 = 0;
                                    } else {
                                        var_r6_4 += 1;
                                        var_r5_3 += 1;
                                        if (var_r5_3 < 4) {
                                            goto loop_90;
                                        }
                                    }
                                    if ((var_r3 != 0) && !(data_020a1154[0].unkC & 0x2000) && !(data_020a1040.unkC & 0x2000)) {
                                        if ((u32) data_02099e18 > 2U) {
                                            /* DEFECT D1's call site. The ROM
                                               leaves func_02040a84's return in
                                               r0 and lets the next bl read it
                                               as its own argument -- the same
                                               idiom src/func_ov007_020cc2cc.c
                                               carries for func_0203da9c. Two
                                               bare calls are the same two bl's
                                               on ARM and an unwritten stack
                                               slot on x86; nesting them is the
                                               only shape that is both. It is
                                               also what the pair MEANS: read
                                               the WM status word, clear bit 0,
                                               write it back. */
                                            func_02040a5c(func_02040a84());
                                        }
                                        data_020a0f2c = 0;
                                        do {
                                        } while (func_0203fd64() != 1);
                                        data_020a1040.unkC = (u16) (data_020a1040.unkC | 0x2000);
                                    }
                                    var_r5_4 = &data_020a1178;
                                    var_r2_2 = 0;
                                    var_r3_2 = 1;
loop_103:
                                    temp_r1_2 = var_r5_4->unkC;
                                    if ((temp_r1_2 & 0x8000) && (temp_r1_2 & 0x4000)) {
                                        var_r2_2 = 1;
                                    } else {
                                        var_r5_4 += 1;
                                        var_r3_2 += 1;
                                        if (var_r3_2 < 4) {
                                            goto loop_103;
                                        }
                                    }
                                    if ((var_r2_2 == 0) && (data_020a1154[0].unkC & 0x2000) && (data_020a1040.unkC & 0x2000)) {
                                        data_020a0f2c = 1;
                                        do {
                                        } while (func_0203fd64() != 1);
                                        data_020a1040.unkC = (u16) (data_020a1040.unkC & 0xDFFF);
                                    }
                                }
                            }
                            var_r0 = 0;
                            var_r1 = data_020a1154;
                            do {
                                if ((var_r1->unkC & 0x8000) && (var_r1->unk0 != data_020a1154[data_020a0f10].unk0)) {
                                    data_020a1040.unk0 = data_020a1154[0].unk0;
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
loop_133:
        if (var_r4->unkC & 0x8000) {
            temp_r1_3 = cur133->unk22;
            temp_r0_9 = var_r4->unk22;
            if (temp_r0_9 != temp_r1_3) {
                data_020a0f1c |= 8;
            } else if (temp_r0_9 != temp_r1_3) {
                data_020a0f1c |= 0x10;
            } else {
                goto block_138;
            }
        } else {
block_138:
            var_r2_3 += 1;
            var_r4 += 1;
            if (var_r2_3 < 4) {
                goto loop_133;
            }
        }
        if ((data_020a1154[0].unkC & 0x2000) || (data_020a1040.unkC & 0x4000)) {
            data_020a0efc = 3;
        }
        if (data_020a0efc != 0) {
            data_020a0f1c &= 0xFF60;
            data_020a1040.unk22 = 0;
            data_020a1040.unk22 = 0;
            data_020a0f30 = 0;
            data_020a1040.unkC = (u16) (data_020a1040.unkC & 0xEFFF);
            data_020a0efc -= 1;
        }
        if (data_020a0efc != 0) {
            return;
        }
        var_r3_3 = data_020a1154;
        var_r4_2 = 1;
        var_r2_4 = 0;
loop_147:
        temp_r1_4 = var_r3_3->unkC;
        if ((temp_r1_4 & 0x8000) && !(temp_r1_4 & 0x1000)) {
            var_r4_2 = 0;
        } else {
            var_r2_4 += 1;
            var_r3_3 += 1;
            if (var_r2_4 < 4) {
                goto loop_147;
            }
        }
        temp_r0 = data_020a1040.unkC & 0x1000;
        if (temp_r0 == 0) {
            return;
        }
        if (var_r4_2 == 0) {
            return;
        }
        data_020a1040.unkC = (u16) (data_020a1040.unkC & 0xEFFF);
        data_020a0f30 = 1;
        return;
    }
    data_020a0f04 = 0;
    data_020a0f1c |= 1;
}
