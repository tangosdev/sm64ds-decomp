#pragma exceptions off

/* storage 0x02384C98 runtime 0x037FCB30 size 0x8E0 wram ARM.
 * Sequence command interpreter: reads command bytes via fn_02385E78 and
 * varints via fn_02385CCC, dispatches on v7 & 0xF0. Jump tables at
 * 0xC0-0xD5 (byte param), 0xB0-0xBD (var ops), 0xFC-0xFF (flow). */

struct Seq {
    unsigned char f0:1, f1:1, f2:1, f3:1, f4:1, f5:1, f6:1, f7:1;
    unsigned char pad01;
    unsigned short f02;
    unsigned char f04, f05;
    signed char f06;
    unsigned char f07, f08;
    unsigned char pad09[5];
    unsigned char f0E, f0F, f10, f11, f12;
    signed char f13;
    unsigned char f14, f15;
    unsigned short f16;
    unsigned char f18, f19, f1A, f1B;
    unsigned short f1C;
    unsigned char pad1E[2];
    int f20;
    int f24;
    int f28;
    int f2C[3];
    unsigned char f38[3];
    unsigned char f3B;
    int f3C;
};

extern int fn_02385E78(struct Seq *);
extern void fn_02385EF0(int);
extern int fn_02385CCC(struct Seq *, unsigned char *, int);
extern void fn_02385578(struct Seq *, unsigned char *, int, int, int);
extern void fn_02385AB0(struct Seq *, unsigned char *, int);
extern int fn_02385DB4(struct Seq *);
extern int fn_023859CC(unsigned char *, int);
extern void fn_023859BC(int, unsigned char *);
extern void fn_02385BC8(int, int, int);
extern int fn_02384BD8(unsigned char *, int);
extern int fn_02383828(void);
extern int fn_0238D594(int, int);

int fn_02384C98(struct Seq *a1, unsigned char *a2) {
    int v5, v7, v6, v9, v10, v13, v14, v15, v16, v18, v19;
    int v11, v24, v26, v29, v32, v33, v34;
    unsigned char v31;
    volatile union {
        unsigned char u;
        signed char s;
    } v35;
    short *v27;
    unsigned char *v30;

    if (a1->f4) {
        if (a1->f3C != 0)
            return 0;
        a1->f4 = 0;
    }
    if (a1->f20 > 0 && --a1->f20 > 0)
        return 0;
    fn_02385EF0(a1->f28);

    while (a1->f20 == 0 && !a1->f4) {
        v5 = 0;
        v6 = 1;
        v7 = fn_02385E78(a1);
        if (v7 == 0xA2) {
            v7 = fn_02385E78(a1);
            v6 = a1->f6;
        }
        if (v7 == 0xA0) {
            v7 = fn_02385E78(a1);
            v32 = 3;
            v5 = 1;
        }
        if (v7 == 0xA1) {
            v7 = fn_02385E78(a1);
            v32 = 4;
            v5 = 1;
        }
        if (!(v7 & 0x80)) {
            v33 = fn_02385E78(a1);
            v9 = fn_02385CCC(a1, a2, v5 ? v32 : 2);
            v10 = v7 + a1->f13;
            if (v6 != 0) {
                if (v10 < 0)
                    v10 = 0;
                else if (v10 > 0x7F)
                    v10 = 0x7F;
                if (!a1->f2)
                    fn_02385578(a1, a2, v10, v33, v9);
                a1->f14 = v10;
                if (a1->f1) {
                    a1->f20 = v9;
                    if (v9 == 0)
                        a1->f4 = 1;
                }
            }
            continue;
        }
        v11 = v7 & 0xF0;
        switch (v11) {
        case 0x80:
            v13 = fn_02385CCC(a1, a2, v5 ? v32 : 2);
            if (v6 != 0) {
                switch (v7) {
                case 0x80:
                    a1->f20 = v13;
                    break;
                case 0x81:
                    if (v13 < 0x10000)
                        a1->f02 = v13;
                    break;
                }
            }
            break;
        case 0x90:
            switch (v7) {
            case 0x93:
                v14 = fn_02385E78(a1);
                v15 = fn_02385DB4(a1);
                if (v6 != 0) {
                    v16 = fn_023859CC(a2, v14);
                    if (v16 != 0 && v16 != (int)a1) {
                        fn_023859BC(v16, a2);
                        fn_02385BC8(v16, a1->f24, v15);
                    }
                }
                break;
            case 0x94:
                v18 = fn_02385DB4(a1);
                if (v6 != 0)
                    a1->f28 = a1->f24 + v18;
                break;
            case 0x95:
                v19 = fn_02385DB4(a1);
                if (v6 != 0 && a1->f3B < 3) {
                    a1->f2C[a1->f3B] = a1->f28;
                    ++a1->f3B;
                    a1->f28 = a1->f24 + v19;
                }
                break;
            }
            break;
        case 0xC0:
        case 0xD0:
            v35.u = fn_02385CCC(a1, a2, v5 ? v32 : 0);
            if (v6 != 0) {
                switch (v7) {
                case 0xC1:
                    a1->f04 = v35.u;
                    break;
                case 0xD5:
                    a1->f05 = v35.u;
                    break;
                case 0xC2:
                    a2[5] = v35.u;
                    break;
                case 0xC5:
                    a1->f07 = v35.u;
                    break;
                case 0xC6:
                    a1->f12 = v35.u;
                    break;
                case 0xC7:
                    a1->f1 = v35.u;
                    break;
                case 0xCF:
                    a1->f15 = v35.u;
                    break;
                case 0xCA:
                    a1->f1A = v35.u;
                    break;
                case 0xCB:
                    a1->f19 = v35.u;
                    break;
                case 0xCC:
                    a1->f18 = v35.u;
                    break;
                case 0xCD:
                    a1->f1B = v35.u;
                    break;
                case 0xD0:
                    a1->f0E = v35.u;
                    break;
                case 0xD1:
                    a1->f0F = v35.u;
                    break;
                case 0xD2:
                    a1->f10 = v35.u;
                    break;
                case 0xD3:
                    a1->f11 = v35.u;
                    break;
                case 0xD4:
                    if (a1->f3B < 3) {
                        a1->f2C[a1->f3B] = a1->f28;
                        a1->f38[a1->f3B] = v35.u;
                        ++a1->f3B;
                    }
                    break;
                case 0xC8:
                    a1->f3 = v35.u;
                    fn_02385AB0(a1, a2, -1);
                    break;
                case 0xC9:
                    a1->f14 = v35.u + a1->f13;
                    a1->f5 = 1;
                    fn_02385AB0(a1, a2, -1);
                    break;
                case 0xCE:
                    a1->f5 = v35.u;
                    fn_02385AB0(a1, a2, -1);
                    break;
                case 0xC3:
                    a1->f13 = v35.s;
                    break;
                case 0xC4:
                    a1->f06 = v35.s;
                    break;
                case 0xC0:
                    a1->f08 = v35.u - 0x40;
                    break;
                }
            }
            break;
        case 0xE0:
            v24 = (short)fn_02385CCC(a1, a2, v5 ? v32 : 1);
            if (v6 != 0) {
                switch (v7) {
                case 0xE1:
                    *(unsigned short *)(a2 + 0x18) = v24;
                    break;
                case 0xE0:
                    a1->f1C = v24;
                    break;
                case 0xE3:
                    a1->f16 = v24;
                    break;
                }
            }
            break;
        case 0xB0:
            v34 = fn_02385E78(a1);
            v26 = (short)fn_02385CCC(a1, a2, v5 ? v32 : 1);
            v27 = (short *)fn_02384BD8(a2, v34);
            if (v6 != 0 && v27 != 0) {
                switch (v7) {
                case 0xB0:
                    *v27 = v26;
                    break;
                case 0xB1:
                    *v27 += v26;
                    break;
                case 0xB2:
                    *v27 -= v26;
                    break;
                case 0xB3:
                    *v27 *= v26;
                    break;
                case 0xB4:
                    if (v26 != 0)
                        *v27 = fn_0238D594(*v27, v26);
                    break;
                case 0xB5:
                    if (v26 >= 0)
                        *v27 <<= v26;
                    else
                        *v27 >>= -v26;
                    break;
                case 0xB6:
                    v29 = fn_02383828();
                    *v27 = (v29 * (v26 + 1)) >> 16;
                    break;
                case 0xB8:
                    a1->f6 = *v27 == v26;
                    break;
                case 0xB9:
                    a1->f6 = *v27 >= v26;
                    break;
                case 0xBA:
                    a1->f6 = *v27 > v26;
                    break;
                case 0xBB:
                    a1->f6 = *v27 <= v26;
                    break;
                case 0xBC:
                    a1->f6 = *v27 < v26;
                    break;
                case 0xBD:
                    a1->f6 = *v27 != v26;
                    break;
                }
            }
            break;
        case 0xF0:
            if (v6 != 0) {
                switch (v7) {
                case 0xFD:
                    if (a1->f3B != 0)
                        a1->f28 = a1->f2C[--a1->f3B];
                    continue;
                case 0xFC:
                    if (a1->f3B != 0) {
                        v30 = (unsigned char *)a1 + (a1->f3B - 1);
                        v31 = v30[0x38];
                        if (v31 != 0 && --v31 == 0) {
                            --a1->f3B;
                        } else {
                            v30[0x38] = v31;
                            a1->f28 = a1->f2C[a1->f3B - 1];
                        }
                    }
                    continue;
                case 0xFE:
                    continue;
                case 0xFF:
                    return -1;
                }
            }
            break;
        }
        continue;
    }
    return 0;
}
