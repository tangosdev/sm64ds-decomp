#pragma opt_strength_reduction off

typedef unsigned short u16;

extern int _ZN4cstd3divEii(int a, int b);
extern int _ZN4cstd3modEii(int a, int b);

void func_ov007_020c49bc(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, u16 *arg6, int arg7)
{
    int temp_sl;
    int temp_sb;
    int sp0;
    int sp4;
    int sp8;
    int spC;
    int sp10;
    int sp14;
    int sp18;
    int sp1C;
    int sp20;
    int sp24;
    int sp28;
    int sp2C;
    int sp34;
    int sp38;
    int sp3C;
    int sp40;
    int sp44;
    int sp48;
    int sp4C;
    int sp30;
    int var_r8;
    int var_r8_2;
    int temp_r7;
    int temp_r6;
    int var_r6;
    int var_r5;
    int temp_r4;
    int temp_r3;
    int temp_r0;
    int temp_r1_5;
    int temp_r0_2;
    int temp_r0_3;
    int temp_r1;
    int temp_r1_2;
    int temp_r1_3;
    int temp_r1_4;
    int temp_r1_6;
    int temp_r7_2;
    int var_r5_2;
    u16 temp_lr;
    u16 temp_r0_4;
    u16 temp_r0_5;
    u16 temp_r1_7;
    u16 temp_r3_2;
    u16 temp_r4_2;
    u16 temp_r4_3;

    temp_sl = arg0 >> 3;
    sp8 = arg1 >> 3;
    sp0 = arg2;
    sp4 = arg3;
    temp_sb = _ZN4cstd3divEii(0x1000, temp_sl);
    spC = _ZN4cstd3divEii(0x1000, sp8);
    if (arg6 == 0) {
        sp10 = 0;
        if (sp8 > 0) {
            sp28 = 0;
            sp2C = arg5 << 0xF;
            sp3C = sp10;
            sp40 = sp10;
            sp34 = 1;
            temp_r7 = arg4 << 0xF;
            sp38 = sp10;
            do {
                *(volatile int *)0x04000500 = sp34;
                temp_r1_2 = sp4 + sp28;
                var_r8 = sp38;
                temp_r0 = spC * (sp8 - sp10);
                if (temp_sl > 0) {
                    temp_r4 = (u16)((temp_r1_2 << 8) >> 0x10) << 0x10;
                    var_r6 = var_r8;
                    temp_r3 = (u16)(((temp_r1_2 + sp2C) << 8) >> 0x10) << 0x10;
                    temp_lr = (u16)(short)temp_r0;
                    var_r5 = var_r8;
                    sp30 = (int)(u16)(short)(temp_r0 - spC);
                    sp4C = temp_lr << 0x10;
                    do {
                        var_r8 += 1;
                        temp_r0_2 = sp0 + var_r6;
                        temp_r1_3 = (temp_r0_2 << 8) >> 0x10;
                        *(volatile int *)0x04000488 = temp_r4 | (u16)temp_r1_3;
                        *(volatile int *)0x0400048C = sp4C | (u16)(short)var_r5;
                        temp_r0_3 = ((temp_r0_2 + temp_r7) << 8) >> 0x10;
                        *(volatile int *)0x0400048C = sp3C;
                        *(volatile int *)0x04000488 = temp_r3 | (u16)temp_r1_3;
                        *(volatile int *)0x0400049C = sp30;
                        *(volatile int *)0x04000488 = temp_r3 | (u16)temp_r0_3;
                        *(volatile int *)0x04000498 = (int)(u16)(short)(temp_sb * var_r8);
                        *(volatile int *)0x04000488 = temp_r4 | (u16)temp_r0_3;
                        *(volatile int *)0x0400049C = (int)temp_lr;
                        var_r6 += temp_r7;
                        var_r5 += temp_sb;
                    } while (var_r8 < temp_sl);
                }
                *(volatile int *)0x04000504 = sp40;
                sp28 += sp2C;
                temp_r1 = sp10 + 1;
                sp10 = temp_r1;
            } while (temp_r1 < sp8);
        }
    } else {
        *(volatile int *)0x04000500 = 1;
        sp14 = 0;
        if (sp8 > 0) {
            sp18 = 0;
            temp_r7_2 = arg7 >> 3;
            sp48 = sp14;
            sp44 = sp14;
            do {
                var_r8_2 = sp44;
                temp_r1_5 = spC * (sp8 - sp14);
                if (temp_sl > 0) {
                    temp_r0_4 = (u16)(short)temp_r1_5;
                    sp24 = (int)temp_r0_4;
                    var_r5_2 = var_r8_2;
                    sp20 = (int)(u16)(short)(temp_r1_5 - spC);
                    sp1C = temp_r0_4 << 0x10;
                    do {
                        temp_r4_2 = arg6[var_r8_2 + sp18];
                        if (temp_r4_2 != 0xFFFF && arg7 > 0) {
                            temp_r6 = _ZN4cstd3modEii(temp_r4_2, temp_r7_2) << 0xF;
                            temp_r1_6 = _ZN4cstd3divEii(temp_r4_2, temp_r7_2) << 0xF;
                            temp_r3_2 = (u16)((temp_r1_6 << 8) >> 0x10);
                            temp_r1_7 = (u16)((temp_r6 << 8) >> 0x10);
                            temp_r4_3 = (u16)(((temp_r6 + 0x8000) << 8) >> 0x10);
                            temp_r0_5 = (u16)(((temp_r1_6 + 0x8000) << 8) >> 0x10);
                            *(volatile int *)0x04000488 = temp_r1_7 | (temp_r3_2 << 0x10);
                            *(volatile int *)0x0400048C = sp1C | (u16)(short)var_r5_2;
                            *(volatile int *)0x0400048C = sp48;
                            *(volatile int *)0x04000488 = temp_r1_7 | (temp_r0_5 << 0x10);
                            *(volatile int *)0x0400049C = sp20;
                            *(volatile int *)0x04000488 = temp_r4_3 | (temp_r0_5 << 0x10);
                            *(volatile int *)0x04000498 = (int)(u16)(short)(temp_sb * (var_r8_2 + 1));
                            *(volatile int *)0x04000488 = temp_r4_3 | (temp_r3_2 << 0x10);
                            *(volatile int *)0x0400049C = sp24;
                        }
                        var_r5_2 += temp_sb;
                        var_r8_2 += 1;
                    } while (var_r8_2 < temp_sl);
                }
                sp18 += temp_sl;
                temp_r1_4 = sp14 + 1;
                sp14 = temp_r1_4;
            } while (temp_r1_4 < sp8);
        }
    }
    *(volatile int *)0x04000504 = 0;
}
