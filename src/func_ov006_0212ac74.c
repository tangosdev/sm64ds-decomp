typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct V2 {
    int x;
    int z;
} V2;

typedef struct Cell {
    int x;
    int z;
    char pad[0x18];
} Cell;

extern void func_0203d6d0(V2 *out, V2 *a, V2 *b);
extern int func_0203d614(V2 *p);
extern void func_02012754(int id);
extern void func_ov004_020b0aa0(int a);
extern void func_ov004_020b0a54(int a);
extern void func_ov004_020b0cac(int a, int a1, int a2, int a3, int arg5, short arg6);
extern void func_ov006_020c3990(char *p);
extern int func_ov006_020c3b80(char *p);
extern void func_ov006_020c3908(char *p);
extern void func_ov006_020c38b0(char *p);
extern void func_ov006_020c3d18(char *p);
extern void func_ov006_0212a654(char *p);
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

#define LB(p) ((int)((unsigned long long)(u32)(int)(p) & 0xffffffffffffffffULL))
#define LA(p) ((int)((long long)(int)(p) & 0xffffffffffffffffLL))

#pragma opt_strength_reduction off
#pragma opt_common_subs off

int func_ov006_0212ac74(char *c)
{
    int i;
    int k;
    int t;
    V2 d1;
    V2 d2;
    V2 d3;

    if (*(u8 *)(c + 0xc4) == 0) {
        *(u8 *)(c + 0xc3) = 1;
        *(u8 *)(c + 0xc4) = 1;
        *(u16 *)(c + 0xc0) = 0;
    }
    if (*(int *)(c + 0x5fd0) > 0) {
        if (*(int *)(c + 0x5fc8) == -1) {
            *(int *)LA(c + 0x5fd0) -= 1;
            if (*(int *)(c + 0x5fd0) <= 0) {
                if (*(int *)(c + 0x5fd8) >= 1)
                    *(int *)(c + 0x5fec) = 2;
            }
        } else {
            *(int *)(c + 0x5fd0) = 0x3c;
        }
    }
    switch (*(int *)(c + 0x5fe8)) {
    case 0:
        *(int *)(c + 0x5fc0) = *(int *)(c + 0x5fb8);
        *(int *)(c + 0x5fc4) = *(int *)(c + 0x5fbc);
        k = data_020a0e40[0];
        if (data_020a0de8[k * 4] != 0) {
            int b = data_020a0deb[k * 4];
            int a = data_020a0dea[k * 4];
            *(int *)(c + 0x5fb8) = (b ? a : a) << 12;
            *(int *)(c + 0x5fbc) = b << 12;
        }
        if (*(int *)(c + 0x5fe4) > 0x14) {
            if (*(int *)(c + 0x5fd8) > 0) {
                for (i = 0; i < *(int *)(c + 0x5fd8); i++) {
                    *(u8 *)(c + i * 0x20 + 0x4f39) = 1;
                    *(u8 *)(c + i * 0x20 + 0x4f3b) = 1;
                }
                *(int *)(c + 0x5fd8) = 0;
                func_02012754(0x10d);
                func_ov004_020b0aa0(0x1d);
            }
        } else if (*(int *)(c + 0x5fc8) < 0) {
            int t;
            if (data_020a0de8[k * 4] != 0 && data_020a0de9[k * 4] != 0)
                t = 1;
            else
                t = 0;
            if (t != 0) {
                int ii;
                V2 *q = (V2 *)(c + 0x4f3c);
                for (ii = 0; ii < 0x16; ii++) {
                    if (*(u8 *)(c + ii * 0x20 + 0x4f38) != 0 &&
                        *(u8 *)(c + ii * 0x20 + 0x4f3a) != 1 &&
                        *(u8 *)(c + ii * 0x20 + 0x4f39) != 1) {
                        int v;
                        func_0203d6d0(&d1, q, (V2 *)(c + 0x5fb8));
                        v = func_0203d614(&d1) < 0x18000 ? 1 : 0;
                        if (v != 0) {
                            func_02012754(0x109);
                            *(u8 *)(c + ii * 0x20 + 0x4f3a) = 1;
                            *(int *)(c + ii * 0x20 + 0x4f44) = 0;
                            *(int *)(c + ii * 0x20 + 0x4f48) = 0;
                            *(int *)(c + 0x5fc8) = ii;
                            *(int *)(c + ii * 0x20 + 0x4f4c) = *(int *)(c + 0x5fb8);
                            *(int *)(c + ii * 0x20 + 0x4f50) = *(int *)(c + 0x5fbc);
                            func_ov004_020b0aa0(0x1d);
                            *(int *)(c + 0x5fec) = 2;
                            break;
                        }
                    }
                    q = (V2 *)((char *)q + 0x20);
                }
                if (*(int *)(c + 0x5fc8) < 0) {
                    int i2;
                    V2 *q2 = (V2 *)(c + 0x4f3c);
                    for (i2 = 0; i2 < 0x16; i2++) {
                        if (*(u8 *)(c + i2 * 0x20 + 0x4f38) != 0 &&
                            *(u8 *)(c + i2 * 0x20 + 0x4f3a) != 1) {
                            int v2;
                            func_0203d6d0(&d2, q2, (V2 *)(c + 0x5fb8));
                            v2 = func_0203d614(&d2) < 0x18000 ? 1 : 0;
                            if (v2 != 0) {
                                *(u8 *)(c + i2 * 0x20 + 0x4f3a) = 1;
                                *(int *)(c + i2 * 0x20 + 0x4f44) = 0;
                                *(int *)(c + i2 * 0x20 + 0x4f48) = 0;
                                *(int *)(c + 0x5fc8) = i2;
                                *(int *)(c + i2 * 0x20 + 0x4f4c) = *(int *)(c + 0x5fb8);
                                *(int *)(c + i2 * 0x20 + 0x4f50) = *(int *)(c + 0x5fbc);
                                break;
                            }
                        }
                        q2 = (V2 *)((char *)q2 + 0x20);
                    }
                }
            }
        } else {
            t = 0;
            if (data_020a0de8[k * 4] != 0) {
                Cell *cells;
                int j;
                func_0203d6d0(&d3, (V2 *)(c + 0x5fb8), (V2 *)(c + 0x5fc0));
                cells = (Cell *)(c + 0x4f3c);
                j = *(int *)(c + 0x5fc8);
                cells[j].x += d3.x;
                *(int *)LB((char *)cells + j * 0x20 + 4) += d3.z;
            } else {
                if (*(u8 *)(c + *(int *)(c + 0x5fc8) * 0x20 + 0x4f39) == 0) {
                    *(u8 *)(c + *(int *)(c + 0x5fc8) * 0x20 + 0x4f39) = 1;
                    if (*(u8 *)(c + *(int *)(c + 0x5fc8) * 0x20 + 0x4f3b) == 0) {
                        t = 1;
                        *(u8 *)(c + *(int *)(c + 0x5fc8) * 0x20 + 0x4f3b) = 1;
                        func_ov006_020c3990(c + 0x51f8);
                    }
                }
                *(u8 *)(c + *(int *)(c + 0x5fc8) * 0x20 + 0x4f3a) = 0;
                *(int *)(c + 0x5fc8) = -1;
            }
            if (t != 0) {
                *(int *)LA(c + 0x5fd8) -= 1;
                if (*(u8 *)(c + 0x5fcc) == 1) {
                    *(u8 *)(c + 0x5fcc) = 0;
                    if (*(int *)(c + 0x5fd8) >= 1) {
                        func_ov004_020b0aa0(0x1d);
                        func_ov004_020b0cac(0x13, 0x80, 0x18, 0, -1, 0xd);
                        *(int *)(c + 0x5fec) = 3;
                        func_02012754(0x104);
                    }
                } else {
                    *(u8 *)(c + 0x5fcc) = 1;
                    if (*(int *)(c + 0x5fd8) >= 1) {
                        func_ov004_020b0aa0(0x1d);
                        func_ov004_020b0cac(0x10, 0x80, 0x18, 0, -1, 0xd);
                        *(int *)(c + 0x5fec) = 1;
                        func_02012754(0x103);
                    }
                }
                *(int *)(c + 0x5fd0) = 0x3c;
            }
        }
        if (*(int *)(c + 0x5fd8) <= 0 && *(u8 *)(c + 0x5fcd) == 1) {
            if (*(int *)(c + 0x5fd4) > 0) {
                *(int *)LA(c + 0x5fd4) -= 1;
                if (*(int *)(c + 0x5fd4) <= 0) {
                    *(int *)(c + 0x5fe8) = 1;
                    func_ov004_020b0a54(0xc);
                }
                if (func_ov006_020c3b80(c + 0x51f8) != 0) {
                    if (*(u8 *)(c + 0x5fcc) == 1)
                        func_ov006_020c3908(c + 0x51f8);
                    else
                        func_ov006_020c38b0(c + 0x51f8);
                }
            } else {
                func_ov004_020b0aa0(0x1d);
                if (*(int *)(c + 0x5fe4) > 0x14) {
                    if (*(u8 *)(c + 0x5fcd) == 1) {
                        func_02012754(0x106);
                        *(int *)(c + 0x5fec) = 4;
                        *(int *)(c + 0x5fd4) = 0x3c;
                    }
                } else {
                if (*(u8 *)(c + 0x5fcc) == 1) {
                    *(int *)(c + 0x5fec) = 0;
                    *(int *)LA(c + 0x5fdc) += 1;
                    *(int *)(c + 0x5fe0) = 0;
                    if (*(int *)(c + 0x5fdc) >= 3) {
                        func_ov004_020b0cac(0x12, 0x80, 0x18, 0, -1, 0xd);
                        func_02012754(0x107);
                        *(int *)LA(c + 0x5ff0) += 3;
                        if (*(int *)(c + 0x5ff0) > 0x270f)
                            *(int *)(c + 0x5ff0) = 0x270f;
                    } else {
                        func_ov004_020b0cac(0x10, 0x80, 0x18, 0, -1, 0xd);
                        func_02012754(0x108);
                        *(int *)LA(c + 0x5ff0) += 1;
                        if (*(int *)(c + 0x5ff0) > 0x270f)
                            *(int *)(c + 0x5ff0) = 0x270f;
                    }
                } else {
                    *(int *)(c + 0x5fec) = 4;
                    *(int *)LA(c + 0x5fe0) += 1;
                    *(int *)(c + 0x5fdc) = 0;
                    if (*(int *)(c + 0x5fe0) >= 3) {
                        func_ov004_020b0cac(0x11, 0x80, 0x18, 0, -1, 0xd);
                        func_02012754(0x105);
                    } else {
                        func_ov004_020b0cac(0x13, 0x80, 0x18, 0, -1, 0xd);
                        func_02012754(0x106);
                    }
                }
                *(int *)(c + 0x5fd4) = 0x3c;
                }
            }
        }
        func_ov006_0212a654(c);
        break;
    case 1:
        if (func_ov006_020c3b80(c + 0x51f8) != 0) {
            if (*(u8 *)(c + 0x5fcc) == 1)
                func_ov006_020c3908(c + 0x51f8);
            else
                func_ov006_020c38b0(c + 0x51f8);
        }
        *(u8 *)(c + 0xc3) = 0;
        func_ov006_0212a654(c);
        break;
    default:
        break;
    }
    func_ov006_020c3d18(c + 0x51f8);
    return 1;
}
