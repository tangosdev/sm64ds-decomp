//cpp
struct Node {
    virtual void dummy();
    virtual void m0();
};

struct Obj;

extern "C" {
extern void SetSubBg0Offset(int a, int b);
extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern int func_ov004_020ad674(void);
extern int func_ov004_020adbc0(void);
extern void func_ov004_020adc1c(void);
extern void func_ov004_020b19f0(void);
extern void func_ov004_020b1a5c(int a0, int a1);
extern void func_ov004_020b1ea4(int a, int b, int c, int d, int e, int f, int g);
extern void func_ov004_020b0d8c(void *c, int a, int b);
extern void func_ov006_02113c14(struct Obj *c);
extern void func_ov006_02114b10(char *sl);
}

extern void *data_ov006_02138ae0[];
extern int data_ov006_0212edfc[];
extern void *data_ov006_0213765c;
extern void *data_ov006_02137a78;
extern void *data_ov006_02137a6c;
extern void *data_ov006_02137a0c[];
extern void *data_ov006_021382e0;
extern void *data_ov006_02138bd0;
extern void *data_ov006_02138bf4;
extern void *data_ov006_02138a40[];
extern void *data_ov006_02138a88[];
extern void *data_ov006_02137a60[];
extern void *data_ov006_0213ecb8[];

static void *ecb_tex(void) {
    return *(void **)((char *)data_ov006_0213ecb8[func_ov004_020ad674()] + 0x54);
}

#pragma opt_strength_reduction off
extern "C" int func_ov006_021173c8(char *sl)
{
    register int sb;
    register int r8;
    int spC;
    int *sp10;
    int sp14;
    int *sp18;
    int *sp1C;
    int sp20;
    int sp24;
    int sp28;
    int sp2C;
    int sp30;
    int sp34;
    int sp38;
    int sp3C;
    volatile int v40;
    int sp40;
    int sp44;
    int sp48;
    int sp4C;
    int sp50;
    int sp54;
    int sp58;
    int sp5C;
    int sp60;
    int sp64;
    int sp68;
    int sp6C;
    int sp70;
    int sp74;
    int sp78;
    int sp7C;

    SetSubBg0Offset(*(int *)(sl + 0x5000 + 0x998), 0);
    sb = 0;
    if ((*(int *)(sl + 0x4000 + 0x660) == 1) && (*(int *)(sl + 0x5000 + 0x998) <= 0)) {
        r8 = 0x18;
        if (*(int *)(sl + 0x5000 + 0x964) > 0) {
            func_ov004_020afdd0(data_ov006_02138ae0[0], 0x30, 0x18, -1, -1);
            func_ov004_020b1ea4(0x80, 0x18, 0x64, 0, 1, 0, 0);
            func_ov004_020afdd0(ecb_tex(), 0x90, 0x18, -1, -1);
            func_ov004_020b1ea4(0xA0, 0x18, *(int *)(sl + 0x5000 + 0x964), 0, 1, 0, 0);
            func_ov004_020b1ea4(0xE0, 0x18, *(int *)(sl + 0x5000 + 0x964) * 0x64, 0, 1, 0, 0);
            r8 += 0x18;
            sb += 1;
        }
        {
            register int r7;
            register int r5;

            r5 = sb * 0x14;
            r7 = 0;
            sp24 = 0;
            sp30 = 0;
            sp38 = 0;
            sp20 = 0x30;
            sp28 = 0x80;
            sp2C = 0x90;
            sp34 = 0xA0;
            sp3C = 0xE0;
            do {
                if ((*(int *)(sl + 0x5000 + 0x960) >= r5) && (*(int *)(sl + r7 * 4 + 0x5968) > 0)) {
                    sp14 = data_ov006_0212edfc[r7];
                    func_ov004_020afdd0(data_ov006_02138ae0[r7 + 1], sp20, r8, -1, -1);
                    func_ov004_020b1ea4(sp28, r8, sp14, sp24, sp24, 1, sp24);
                    func_ov004_020afdd0(ecb_tex(), sp2C, r8, -1, -1);
                    sp10 = (int *)(sl + r7 * 4 + 0x5968);
                    func_ov004_020b1ea4(sp34, r8, *sp10, sp30, sp30, sp30, sp30);
                    func_ov004_020b1ea4(sp3C, r8, sp14 * *sp10, sp38, sp38, 1, sp38);
                    r8 += 0x18;
                    r5 += 0x14;
                    sb += 1;
                }
                r7 += 1;
            } while (r7 < 4);
        }
        if ((*(int *)(sl + 0x5000 + 0x97C) > 0) && (*(int *)(sl + 0x5000 + 0x960) >= sb * 0x14)) {
            func_ov004_020afdd0(data_ov006_0213765c, 0x30, r8, -1, -1);
            func_ov004_020b1ea4(0x80, r8, 0x12C, 0, 1, 0, 0);
            func_ov004_020afdd0(ecb_tex(), 0x90, r8, -1, -1);
            func_ov004_020b1ea4(0xA0, r8, *(int *)(sl + 0x5000 + 0x97C), 0, 0, 0, 0);
            func_ov004_020b1ea4(0xE0, r8, *(int *)(sl + 0x5000 + 0x97C) * 0x12C, 0, 1, 0, 0);
            r8 += 0x18;
            sb += 1;
        }
        if ((*(int *)(sl + 0x5000 + 0x984) > 0) && (*(int *)(sl + 0x5000 + 0x960) >= sb * 0x14)) {
            func_ov004_020afdd0(data_ov006_02137a78, 0x30, r8, -1, -1);
            func_ov004_020b1ea4(0x80, r8, 0xBB8, 0, 1, 0, 0);
            func_ov004_020afdd0(ecb_tex(), 0x90, r8, -1, -1);
            func_ov004_020b1ea4(0xA0, r8, *(int *)(sl + 0x5000 + 0x984), 0, 0, 0, 0);
            func_ov004_020b1ea4(0xE0, r8, *(int *)(sl + 0x5000 + 0x984) * 0xBB8, 0, 1, 0, 0);
            r8 += 0x18;
            sb += 1;
        }
        if ((*(int *)(sl + 0x5000 + 0x980) > 0) && (*(int *)(sl + 0x5000 + 0x960) >= sb * 0x14)) {
            func_ov004_020afdd0(data_ov006_02137a6c, 0x30, r8, -1, -1);
            func_ov004_020b1ea4(0x80, r8, 0x7D0, 0, 1, 0, 0);
            func_ov004_020afdd0(ecb_tex(), 0x90, r8, -1, -1);
            func_ov004_020b1ea4(0xA0, r8, *(int *)(sl + 0x5000 + 0x980), 0, 0, 0, 0);
            func_ov004_020b1ea4(0xE0, r8, *(int *)(sl + 0x5000 + 0x980) * 0x7D0, 0, 1, 0, 0);
            r8 += 0x18;
            sb += 1;
        }
        {
            register int r7;
            register int r5;
            int r11;

            sp18 = (int *)0;
            spC = sb * 0x14;
            v40 = 0x30;
            sp40 = v40;
            sp44 = 0x40;
            sp48 = 0x1F40;
            r11 = 0x20;
            r7 = 0;
            sp50 = 1;
            sp54 = 0x80;
            sp58 = 0x90;
            sp60 = 0xA0;
            sp68 = 0xE0;
            sp4C = 0;
            sp5C = 0;
            sp64 = 0;
            do {
                sp1C = (int *)(sl + r7 * 4 + 0x5988);
                if ((*sp1C > 0) && (*(int *)(sl + 0x5000 + 0x960) >= spC)) {
                    register int r6;

                    r5 = -1;
                    switch (r7) {
                case 0:
                    func_ov004_020afdd0(data_ov006_02137a0c[1], r11, r8, r5, r5);
                    func_ov004_020afdd0(data_ov006_02137a0c[1], sp40, r8, r5, r5);
                    func_ov004_020afdd0(data_ov006_02137a0c[1], sp44, r8, r5, r5);
                    r6 = 0x1388;
                    break;
                case 1:
                    func_ov004_020afdd0(data_ov006_02137a0c[0], r11, r8, r5, r5);
                    func_ov004_020afdd0(data_ov006_02137a0c[0], sp40, r8, r5, r5);
                    func_ov004_020afdd0(data_ov006_02137a0c[0], sp44, r8, r5, r5);
                    r6 = sp48;
                    break;
                case 2:
                    func_ov004_020afdd0(data_ov006_02137a0c[2], r11, r8, r5, r5);
                    func_ov004_020afdd0(data_ov006_02137a0c[2], sp40, r8, r5, r5);
                    func_ov004_020afdd0(data_ov006_02137a0c[2], sp44, r8, r5, r5);
                    r6 = 0x2710;
                    break;
                }
                func_ov004_020b1ea4(sp54, r8, r6, sp4C, sp4C, sp50, sp4C);
                func_ov004_020afdd0(ecb_tex(), sp58, r8, r5, r5);
                func_ov004_020b1ea4(sp60, r8, *sp1C, sp5C, sp5C, sp5C, sp5C);
                func_ov004_020b1ea4(sp68, r8, r6 * *sp1C, sp64, sp64, sp50, sp64);
                r8 += 0x18;
                spC += 0x14;
                    sb += 1;
                }
                r7 += 1;
            } while (r7 < 3);
        }
        if ((*(int *)(sl + 0x5000 + 0x978) > 0) && (*(int *)(sl + 0x5000 + 0x960) >= sb * 0x14)) {
            func_ov004_020afdd0(data_ov006_02138ae0[5], 0x30, r8, -1, -1);
            func_ov004_020b1ea4(0x80, r8, 0x2710, 0, 1, 0, 0);
            func_ov004_020afdd0(ecb_tex(), 0x90, r8, -1, -1);
            func_ov004_020b1ea4(0xA0, r8, *(int *)(sl + 0x5000 + 0x978), 0, 0, 0, 0);
            func_ov004_020b1ea4(0xE0, r8, *(int *)(sl + 0x5000 + 0x978) * 0x2710, 0, 1, 0, 0);
            r8 += 0x18;
            sb += 1;
        }
        if ((*(int *)(sl + 0x5000 + 0x994) > 0) && (*(int *)(sl + 0x5000 + 0x960) >= sb * 0x14)) {
            func_ov004_020afdd0(data_ov006_021382e0, 0x30, r8, -1, -1);
            func_ov004_020b1ea4(0x80, r8, 0x7D0, 0, 1, 0, 0);
            func_ov004_020afdd0(ecb_tex(), 0x90, r8, -1, -1);
            func_ov004_020b1ea4(0xA0, r8, *(int *)(sl + 0x5000 + 0x994), 0, 0, 0, 0);
            func_ov004_020b1ea4(0xE0, r8, *(int *)(sl + 0x5000 + 0x994) * 0x7D0, 0, 1, 0, 0);
            r8 += 0x18;
            sb += 1;
        }
        if (*(int *)(sl + 0x5000 + 0x960) >= sb * 0x14) {
            if (*(unsigned char *)(sl + 0x8) == 0) {
                func_ov004_020afdd0(data_ov006_02138bd0, 0x40, r8, -1, -1);
            } else {
                func_ov004_020afdd0(data_ov006_02138bf4, 0x40, r8, -1, -1);
            }
            func_ov004_020b1ea4(0xE0, r8, func_ov004_020adbc0(), 0, 1, 0, 0);
            sb += 1;
            if (*(int *)(sl + 0x5000 + 0x960) < 0x118) {
                *(int *)(sl + 0x5000 + 0x960) = 0x118;
            }
        }
        if ((*(int *)(sl + 0x4000 + 0x660) == 1) && (*(int *)(sl + 0x5000 + 0x960) >= 0x12C)) {
            if (sb >= 7) {
                func_ov004_020b0d8c(sl, 0x80, 0xA8);
            } else if (sb == 6) {
                func_ov004_020b0d8c(sl, 0xE0, 0xA8);
            } else {
                func_ov004_020b0d8c(sl, 0xE0, 0xA0);
            }
        }
    }
    if ((*(unsigned char *)(sl + 0x8) != 0) || (*(int *)(sl + 0x5000 + 0x95C) == 0)) {
        func_ov006_02114b10(sl);
    }
    func_ov004_020b1a5c(func_ov004_020adbc0(), 6);
    func_ov004_020adc1c();
    func_ov004_020b19f0();

    sb = 0;
    sp6C = sb;
    sp70 = sb;
    sp74 = sb;
    sp7C = sb;
    sp78 = sb;
    {
        register int r2;
        register int r3;
        register int r5;
        register int r4;
        register int r0_3;

        r5 = -1;
        do {
            r2 = *(int *)(sl + sb * 4 + 0x478C);
            if (r2 > 0) {
                sp18 = (int *)(sl + sb * 4 + 0x47A0);
                if (*sp18 >= 2) {
                    func_ov004_020b1ea4((*(int *)(sl + sb * 8 + 0x47C8) >> 0xC) - 0x10,
                                        *(int *)(sl + sb * 8 + 0x47CC) >> 0xC,
                                        r2, r5, sp6C, sp6C, sp6C);
                    r2 = *(int *)(sl + sb * 4 + 0x478C);
                    r3 = sp70;
                    if (r2 != 0) {
                        do {
                            r2 = (unsigned int)(((unsigned long long)r2 * 0xCCCCCCCDULL) >> 35);
                            r3 += 1;
                        } while (r2 != 0);
                    }
                    func_ov004_020afdd0(ecb_tex(),
                                        ((*(int *)(sl + sb * 8 + 0x47C8) >> 0xC) + (r3 * 8)) - 8,
                                        *(int *)(sl + sb * 8 + 0x47CC) >> 0xC, r5, r5);
                    r3 = *(int *)(sl + sb * 4 + 0x478C);
                    r2 = sp74;
                    if (r3 != 0) {
                        do {
                            r3 = (int)((unsigned int)(((unsigned long long)r3 * 0xCCCCCCCDULL) >> 35));
                            r2 += 1;
                        } while (r3 != 0);
                    }
                    func_ov004_020b1ea4((*(int *)(sl + sb * 8 + 0x47C8) >> 0xC) + (r2 * 8) + 8,
                                        *(int *)(sl + sb * 8 + 0x47CC) >> 0xC,
                                        *sp18, r5, sp78, sp78, sp78);
                } else {
                    func_ov004_020b1ea4(*(int *)(sl + sb * 8 + 0x47C8) >> 0xC,
                                        *(int *)(sl + sb * 8 + 0x47CC) >> 0xC,
                                        r2, r5, sp7C, sp7C, sp7C);
                }
            }
            sb += 1;
        } while (sb < 5);
        r4 = 0;
        do {
            if (*(int *)(sl + r4 + 0x4804) == 1) {
                r0_3 = ((unsigned int)*(int *)(sl + r4 * 4 + 0x4814) >> 0x1F) + (*(int *)(sl + r4 * 4 + 0x4814) / 5);
                if (r0_3 > 3) {
                    r0_3 = 2;
                }
                func_ov004_020afdd0(data_ov006_02137a60[r0_3],
                                    *(int *)(sl + r4 * 8 + 0x4854) >> 0xC,
                                    *(int *)(sl + r4 * 8 + 0x4858) >> 0xC, r5, 0);
            }
            r4 += 1;
        } while (r4 < 0x10);
    }
    {
        int r0_4;
        int r0_5;
        int r2;
        int rem;

        r0_4 = 0;
        r2 = *(int *)(sl + 0x5000 + 0x958);
        if (r2 > 0) {
            rem = r2 - (0x1E * (((unsigned int)r2 >> 0x1F) + ((int)(r2 + (int)(((long long)0x88888889LL * r2) >> 32)) >> 4)));
            if (rem > 0xF) {
                r0_4 = 1;
            }
        } else if (*(int *)(sl + 0x4784) > 0) {
            r0_4 = 1;
        }
        if (*(unsigned char *)(sl + 0x8) == 0) {
            func_ov004_020afdd0(data_ov006_02138a40[r0_4], 0x18, -0x90, -1, 0);
        } else {
            func_ov004_020afdd0(data_ov006_02138a88[r0_4], 0x18, -0x90, -1, 0);
        }
        r0_5 = 0;
        r2 = *(int *)(sl + 0x5000 + 0x958);
        if (r2 > 0) {
            rem = r2 - (0x1E * (((unsigned int)r2 >> 0x1F) + ((int)(r2 + (int)(((long long)0x88888889LL * r2) >> 32)) >> 4)));
            if (rem > 0xF) {
                r0_5 = 1;
            }
        } else if (*(int *)(sl + 0x4788) > 0) {
            r0_5 = 1;
        }
        if (*(unsigned char *)(sl + 0x8) == 0) {
            func_ov004_020afdd0(data_ov006_02138a40[r0_5], 0xE8, -0x90, -1, 0);
        } else {
            func_ov004_020afdd0(data_ov006_02138a88[r0_5], 0xE8, -0x90, -1, 0);
        }
    }
    {
        int r4;
        Node *n;

        r4 = 0;
        if (*(int *)(sl + 0x4674) > 0) {
            do {
                n = *(Node **)(sl + r4 * 4 + 0x4740);
                n->m0();
                r4 += 1;
            } while (r4 < *(int *)(sl + 0x4674));
        }
        n = *(Node **)(sl + 0x4778);
        if (n != 0) {
            n->m0();
        }
        r4 = 0;
        if (*(int *)(sl + 0x4668) > 0) {
            do {
                n = *(Node **)(sl + r4 * 4 + 0x4688);
                n->m0();
                r4 += 1;
            } while (r4 < *(int *)(sl + 0x4668));
        }
        r4 = 0;
        if (*(int *)(sl + 0x466C) > 0) {
            do {
                n = *(Node **)(sl + r4 * 4 + 0x4720);
                n->m0();
                r4 += 1;
            } while (r4 < *(int *)(sl + 0x466C));
        }
        r4 = 0;
        if (*(int *)(sl + 0x4670) > 0) {
            do {
                n = *(Node **)(sl + r4 * 4 + 0x46BC);
                n->m0();
                r4 += 1;
            } while (r4 < *(int *)(sl + 0x4670));
        }
        r4 = 0;
        if (*(int *)(sl + 0x4678) > 0) {
            do {
                n = *(Node **)(sl + r4 * 4 + 0x474C);
                n->m0();
                r4 += 1;
            } while (r4 < *(int *)(sl + 0x4678));
        }
        r4 = 0;
        if (*(int *)(sl + 0x467C) > 0) {
            do {
                n = *(Node **)(sl + r4 * 4 + 0x4764);
                n->m0();
                r4 += 1;
            } while (r4 < *(int *)(sl + 0x467C));
        }
        r4 = 0;
        if (*(int *)(sl + 0x4680) > 0) {
            do {
                n = *(Node **)(sl + r4 * 4 + 0x4770);
                n->m0();
                r4 += 1;
            } while (r4 < *(int *)(sl + 0x4680));
        }
        (*(Node **)(sl + 0x4684))->m0();
        n = *(Node **)(sl + 0x477C);
        if (n != 0) {
            n->m0();
        }
        n = *(Node **)(sl + 0x4780);
        if (n != 0) {
            n->m0();
        }
    }
    {
        int i;

        i = 0;
        if (*(int *)(sl + 0x4668) > 0) {
            do {
                func_ov006_02113c14((struct Obj *)*(int *)(sl + i * 4 + 0x4688));
                i += 1;
            } while (i < *(int *)(sl + 0x4668));
        }
    }
    if ((*(unsigned char *)(sl + 0x8) == 0) && (*(int *)(sl + 0x5000 + 0x95C) != 0)) {
        func_ov006_02114b10(sl);
    }
    return 1;
}
