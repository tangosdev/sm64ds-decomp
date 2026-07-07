typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef signed char s8;
typedef unsigned char u8;
typedef long long s64;

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern s64 func_0203d5bc(int *p);
extern void func_0203d388(int *p, int angle);
extern int func_0203d434(int *p);
extern void func_0203d480(int *out, int *p);
extern int func_0203d570(int *a, int *b);
extern int func_0203d614(int *p);
extern void func_0203d630(int *p, int m);
extern void func_0203d6d0(int *o, int *a, int *b);
extern void func_ov006_0211470c(int *a, int *b);
extern void func_ov006_021146f4(int *a, char *b);
extern int func_ov006_021115c4(unsigned char *p);
extern void func_ov006_021115cc(char *p);
extern int func_ov006_02111dcc(char *p, int val);
extern void func_ov006_02111b90(char *p, int id, int *vel);
extern int func_ov006_02111e90(char *p, int *arg);
extern int func_ov006_02111ee8(char *p, int *arg);
extern int func_ov006_02111f8c(char *p, int *arg);
extern int func_ov006_02112030(char *p, int *arg);
extern int func_ov006_021120d4(char *p, int *arg);
extern int func_ov006_02112190(char *p, int *arg);
extern int func_ov006_021122e0(char *p, int *arg);
extern int func_ov006_0211248c(char *p, int *arg);
extern int func_ov006_02112504(char *p, int *arg);
extern int func_ov006_021126b4(char *p, int *arg);
extern void func_020126ac(int a, int b, int c, int d, int e);
extern int func_020126e8(int a);
extern void func_02012718(int a, int b);

#define MGR(c) (*(char **)((c) + 4))
#define MGR_I(c, o) (*(int *)(MGR(c) + (o)))
#define MGR_P(c, o) (*(char **)(MGR(c) + (o)))
#define SELF_I(c, o) (*(int *)((c) + (o)))
#define SELF_B(c, o) (*(unsigned char *)((c) + (o)))
#define SELF_SB(c, o) (*(signed char *)((c) + (o)))
#define FIXMUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))
#define RAND15() ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff)

static int ang_idx(int v) { return (s16)(v + ((v >> 4) >> 27) << 11) >> 16; }

void func_ov006_02112ad8(register char *self) {
    char spac[0x194];
    s32 j;

    {
        s32 v20;
        v20 = SELF_I(self, 0x20);
        *(s32*)(spac + 0x5C) = v20;
    }
    *(s32*)(spac + 0x4) = 0;
    *(s32*)(spac + 0x60) = *(s32*)(self + 0x24);
    SELF_I(self, 0xf8) = -1;
    *(s32*)(spac + 0x8) = *(s32*)(spac + 0x4);
    *(s32*)(spac + 0xC) = *(s32*)(spac + 0x4);
    *(s32*)(spac + 0x10) = *(s32*)(spac + 0x4);
    *(s32*)(spac + 0x14) = *(s32*)(spac + 0x4);
    *(s32*)(spac + 0x18) = *(s32*)(spac + 0x4);
    *(s32*)(spac + 0x20) = *(s32*)(spac + 0x4);
    *(s32*)(spac + 0x1C) = *(s32*)(spac + 0x4);
    j = *(s32*)(spac + 0x1C);
    while (j < 0x20) {
        SELF_SB(self, 0x6c + j) = (s8)*(s32*)(spac + 0x1C);
        SELF_SB(self, 0xac + j) = (s8)*(s32*)(spac + 0x1C);
        SELF_SB(self, 0x8c + j) = (s8)*(s32*)(spac + 0x1C);
        j += 1;
    }
    SELF_SB(self, 0xcc) = (s8)*(s32*)(spac + 0x1C);
    SELF_SB(self, 0xcd) = (s8)*(s32*)(spac + 0x1C);
    if (SELF_I(self, 0xc) >= 0x77000) {
        j = 0;
        while (j < 0x19) {
            SELF_SB(self, 0xce + j) = 0;
            j += 1;
        }
    } else {
        j = 0;
        while (j < 0x19) {
            SELF_SB(self, 0xce + j) = 0;
            if (j < MGR_I(self, 0x4670)) {
                *(s32*)(spac + 0x110) = (s32) MGR_P(self, 0x46bc + j * 4);
                *(s32*)(spac + 0x64) = SELF_I(self, 0x8);
                *(s32*)(spac + 0x68) = SELF_I(self, 0xc);
                func_ov006_0211470c((s32*)(spac + 0xAC), (int *)(*(s32*)(spac + 0x110)));
                *(s32*)(spac + 0x64) -= *(s32*)(spac + 0xAC);
                *(s32*)(spac + 0x68) -= *(s32*)(spac + 0xB0);
                if (func_0203d5bc((s32*)(spac + 0x64)) <= 0x900000) {
                    SELF_SB(self, 0xce + j) = 1;
                    *(s32*)(spac + 0x1C) = 1;
                }
            }
            j += 1;
        }
    }
    *(s32*)(spac + 0x24) = 0;
    *(s32*)(spac + 0x2C) = 0;
    *(s32*)(spac + 0x34) = 0;
    *(s32*)(spac + 0x38) = 0;
    *(s32*)(spac + 0x3C) = 0;
    *(s32*)(spac + 0x40) = 0;
    *(s32*)(spac + 0x44) = 0;
    *(s32*)(spac + 0x4C) = 0;
    *(s32*)(spac + 0x50) = 0x800;
    *(s32*)(spac + 0x30) = 0;
    *(s32*)(spac + 0x48) = 0;
    *(s32*)(spac + 0x28) = 0;
    {
        s32 ang;
        s32 idx;
loop_15:
    ang = *(s32*)(spac + 0x28);
    *(s32*)(spac + 0x1C) = 0;
    idx = ang;
    do {
        SELF_SB(self, 0x4c + idx) = (s8)*(s32*)(spac + 0x2C);
        *(s32*)(spac + 0x54) = *(s32*)(spac + 0x2C);
        *(s32*)(spac + 0x58) = *(s32*)(spac + 0x2C);
        *(s32*)(spac + 0x58) = *(s32*)(spac + 0x2C) + SELF_I(self, 0x28);
        func_0203d388((s32*)(spac + 0x54), ang);
        if (func_0203d570((s32*)(spac + 0x54), (int *)(self + 0x20)) >= 0) {
            *(s32*)(spac + 0x54) += SELF_I(self, 0x8);
            *(s32*)(spac + 0x58) += SELF_I(self, 0xc);
            if (SELF_B(MGR(self), 0x8) == 0) {
                *(s32*)(spac + 0xB4) = *(s32*)(spac + 0x54);
                *(s32*)(spac + 0xB8) = *(s32*)(spac + 0x58);
                if (func_ov006_021126b4(self, (s32*)(spac + 0xB4)) != 0) {
                    SELF_SB(self, 0x4c + idx) = 1;
                    SELF_SB(self, 0x6c + idx) = 1;
                    SELF_SB(self, 0xcc) = 1;
                    *(s32*)(spac + 0x1C) = 1;
                } else {
                    *(s32*)(spac + 0xBC) = *(s32*)(spac + 0x54);
                    *(s32*)(spac + 0xC0) = *(s32*)(spac + 0x58);
                    if (func_ov006_02112504(self, (s32*)(spac + 0xBC)) != 0) {
                        SELF_SB(self, 0x4c + idx) = 1;
                        SELF_SB(self, 0x6c + idx) = 1;
                        SELF_SB(self, 0xcc) = 1;
                        *(s32*)(spac + 0x4) = 1;
                        *(s32*)(spac + 0x1C) = 1;
                    } else {
                        *(s32*)(spac + 0xC4) = *(s32*)(spac + 0x54);
                        *(s32*)(spac + 0xC8) = *(s32*)(spac + 0x58);
                        if (func_ov006_021122e0(self, (s32*)(spac + 0xC4)) != 0) {
                            SELF_SB(self, 0x4c + idx) = 1;
                            SELF_SB(self, 0x6c + idx) = 1;
                            SELF_SB(self, 0xcc) = 1;
                            SELF_SB(self, 0x8c + idx) = 1;
                            *(s32*)(spac + 0xC) = 1;
                            *(s32*)(spac + 0x1C) = 1;
                        }
                    }
                }
            } else {
                *(s32*)(spac + 0xCC) = *(s32*)(spac + 0x54);
                *(s32*)(spac + 0xD0) = *(s32*)(spac + 0x58);
                if (func_ov006_021126b4(self, (s32*)(spac + 0xCC)) != 0) {
                    SELF_SB(self, 0x4c + idx) = 1;
                    SELF_SB(self, 0x6c + idx) = 1;
                    SELF_SB(self, 0xcc) = 1;
                    *(s32*)(spac + 0x1C) = 1;
                } else {
                    *(s32*)(spac + 0xD4) = *(s32*)(spac + 0x54);
                    *(s32*)(spac + 0xD8) = *(s32*)(spac + 0x58);
                    if (func_ov006_021122e0(self, (s32*)(spac + 0xD4)) != 0) {
                        SELF_SB(self, 0x4c + idx) = 1;
                        SELF_SB(self, 0x6c + idx) = 1;
                        SELF_SB(self, 0xcc) = 1;
                        SELF_SB(self, 0x8c + idx) = 1;
                        *(s32*)(spac + 0xC) = 1;
                        *(s32*)(spac + 0x1C) = 1;
                    } else if (*(s32*)(spac + 0x1C) == 1) {
                        *(s32*)(spac + 0xDC) = *(s32*)(spac + 0x54);
                        *(s32*)(spac + 0xE0) = *(s32*)(spac + 0x58);
                        if (func_ov006_02112190(self, (s32*)(spac + 0xDC)) != 0) {
                            SELF_SB(self, 0x4c + idx) = 1;
                            SELF_SB(self, 0x6c + idx) = 1;
                            SELF_SB(self, 0xcc) = 1;
                            SELF_SB(self, 0xcd) = 1;
                            *(s32*)(spac + 0x10) = 1;
                            *(s32*)(spac + 0x1C) = 1;
                        } else {
                            *(s32*)(spac + 0xE4) = *(s32*)(spac + 0x54);
                            *(s32*)(spac + 0xE8) = *(s32*)(spac + 0x58);
                            if (func_ov006_02112030(self, (s32*)(spac + 0xE4)) != 0) {
                                SELF_SB(self, 0x4c + idx) = 1;
                                SELF_SB(self, 0x6c + idx) = 1;
                                SELF_SB(self, 0xcc) = 1;
                                *(s32*)(spac + 0x1C) = 1;
                            } else {
                                *(s32*)(spac + 0xEC) = *(s32*)(spac + 0x54);
                                *(s32*)(spac + 0xF0) = *(s32*)(spac + 0x58);
                                if (func_ov006_02111f8c(self, (s32*)(spac + 0xEC)) != 0) {
                                    SELF_SB(self, 0x4c + idx) = 1;
                                    SELF_SB(self, 0x6c + idx) = 1;
                                    SELF_SB(self, 0xcc) = 1;
                                    *(s32*)(spac + 0x1C) = 1;
                                } else {
                                    *(s32*)(spac + 0xF4) = *(s32*)(spac + 0x54);
                                    *(s32*)(spac + 0xF8) = *(s32*)(spac + 0x58);
                                    if (func_ov006_02111ee8(self, (s32*)(spac + 0xF4)) != 0) {
                                        SELF_SB(self, 0x4c + idx) = 1;
                                        SELF_SB(self, 0x6c + idx) = 1;
                                        SELF_SB(self, 0xcc) = 1;
                                        *(s32*)(spac + 0x1C) = 1;
                                    } else {
                                        *(s32*)(spac + 0xFC) = *(s32*)(spac + 0x54);
                                        *(s32*)(spac + 0x100) = *(s32*)(spac + 0x58);
                                        if (func_ov006_02111e90(self, (s32*)(spac + 0xFC)) != 0) {
                                            SELF_SB(self, 0x4c + idx) = 1;
                                            SELF_SB(self, 0x6c + idx) = 1;
                                            SELF_SB(self, 0xcc) = 1;
                                            *(s32*)(spac + 0x1C) = 1;
                                            *(s32*)(spac + 0x14) = 1;
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        *(s32*)(spac + 0xE4) = *(s32*)(spac + 0x54);
                        *(s32*)(spac + 0xE8) = *(s32*)(spac + 0x58);
                        if (func_ov006_02112030(self, (s32*)(spac + 0xE4)) != 0) {
                            SELF_SB(self, 0x4c + idx) = 1;
                            SELF_SB(self, 0x6c + idx) = 1;
                            SELF_SB(self, 0xcc) = 1;
                            *(s32*)(spac + 0x1C) = 1;
                        } else {
                            *(s32*)(spac + 0xEC) = *(s32*)(spac + 0x54);
                            *(s32*)(spac + 0xF0) = *(s32*)(spac + 0x58);
                            if (func_ov006_02111f8c(self, (s32*)(spac + 0xEC)) != 0) {
                                SELF_SB(self, 0x4c + idx) = 1;
                                SELF_SB(self, 0x6c + idx) = 1;
                                SELF_SB(self, 0xcc) = 1;
                                *(s32*)(spac + 0x1C) = 1;
                            } else {
                                *(s32*)(spac + 0xF4) = *(s32*)(spac + 0x54);
                                *(s32*)(spac + 0xF8) = *(s32*)(spac + 0x58);
                                if (func_ov006_02111ee8(self, (s32*)(spac + 0xF4)) != 0) {
                                    SELF_SB(self, 0x4c + idx) = 1;
                                    SELF_SB(self, 0x6c + idx) = 1;
                                    SELF_SB(self, 0xcc) = 1;
                                    *(s32*)(spac + 0x1C) = 1;
                                } else {
                                    *(s32*)(spac + 0xFC) = *(s32*)(spac + 0x54);
                                    *(s32*)(spac + 0x100) = *(s32*)(spac + 0x58);
                                    if (func_ov006_02111e90(self, (s32*)(spac + 0xFC)) != 0) {
                                        SELF_SB(self, 0x4c + idx) = 1;
                                        SELF_SB(self, 0x6c + idx) = 1;
                                        SELF_SB(self, 0xcc) = 1;
                                        *(s32*)(spac + 0x1C) = 1;
                                        *(s32*)(spac + 0x14) = 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            *(s32*)(spac + 0x54) += SELF_I(self, 0x8);
            *(s32*)(spac + 0x58) += SELF_I(self, 0xc);
        }
        if (*(s32*)(spac + 0x1C) == 0) {
            *(s32*)(spac + 0x104) = *(s32*)(spac + 0x54);
            *(s32*)(spac + 0x108) = *(s32*)(spac + 0x58);
            if (func_ov006_0211248c(self, (s32*)(spac + 0x104)) != 0) {
                SELF_SB(self, 0x4c + idx) = 1;
                SELF_SB(self, 0x6c + idx) = 1;
                SELF_SB(self, 0xcc) = 1;
                *(s32*)(spac + 0x1C) = 1;
                if (ang == 0)
                    *(s32*)(spac + 0x18) = 1;
            } else {
                *(s32*)(spac + 0x10C) = *(s32*)(spac + 0x54);
                *(s32*)(spac + 0x110) = *(s32*)(spac + 0x58);
                if (func_ov006_021120d4(self, (s32*)(spac + 0x10C)) != 0) {
                    SELF_SB(self, 0x4c + idx) = 1;
                    SELF_SB(self, 0x6c + idx) = 1;
                    SELF_SB(self, 0xcc) = 1;
                    *(s32*)(spac + 0x1C) = 1;
                    *(s32*)(spac + 0x8) = 1;
                }
            }
        }
        ang = (s16)(ang + 0x800);
        idx += 1;
    } while (idx < 0x20);
    if (*(s32*)(spac + 0x20) < 0x11 && SELF_B(self, 0x3a) == 0) {
        j = *(s32*)(spac + 0x30);
        if (MGR_I(self, 0x4668) > 0) {
            do {
                if (j != SELF_I(self, 0x2c)) {
                    *(s32*)(spac + 0x110) = (s32)((j >= 0xd) ? *(s32*)(spac + 0x34) : MGR_P(self, 0x4688 + j * 4));
                    if (SELF_B((char *)*(s32*)(spac + 0x110), 0x3a) == 0) {
                        *(s32*)(spac + 0x110) = (s32)((j >= 0xd) ? *(s32*)(spac + 0x38) : MGR_P(self, 0x4688 + j * 4));
                        if (SELF_B((char *)*(s32*)(spac + 0x110), 0x30) != 0) {
                            *(s32*)(spac + 0x110) = (s32)((j < 0xd) ? MGR_P(self, 0x4688 + j * 4) : (char *)(spac + 0x3C));
                            func_ov006_0211470c((s32*)(spac + 0x114), (int *)*(s32*)(spac + 0x110));
                            func_0203d6d0((s32*)(spac + 0x11C), (int *)(self + 0x8), (s32*)(spac + 0x114));
                            *(s32*)(spac + 0x70) = *(s32*)(spac + 0x120);
                            *(s32*)(spac + 0x6C) = *(s32*)(spac + 0x11C);
                            if (func_0203d5bc((s32*)(spac + 0x6C)) < 0x100000) {
                                *(s32*)(spac + 0x110) = (s32)((j < 0xd) ? MGR_P(self, 0x4688 + j * 4) : (char *)(spac + 0x3C));
                                func_ov006_0211470c((s32*)(spac + 0x124), (int *)*(s32*)(spac + 0x110));
                                *(s32*)(spac + 0x74) = *(s32*)(spac + 0x124);
                                *(s32*)(spac + 0x78) = *(s32*)(spac + 0x128);
                                if (SELF_I(self, 0xc) < 0x90000 || SELF_I(self, 0x8) <= *(s32*)(spac + 0x124)) {
                                    ang = *(s32*)(spac + 0x44);
                                    do {
                                        *(s32*)(spac + 0x58) = SELF_I(self, 0x28);
                                        *(s32*)(spac + 0x54) = 0;
                                        func_0203d388((s32*)(spac + 0x54), *(s32*)(spac + 0x40));
                                        *(s32*)(spac + 0x54) += SELF_I(self, 0x8);
                                        *(s32*)(spac + 0x58) += SELF_I(self, 0xc);
                                        func_0203d6d0((s32*)(spac + 0x12C), (s32*)(spac + 0x54), (s32*)(spac + 0x74));
                                        *(s32*)(spac + 0x70) = *(s32*)(spac + 0x130);
                                        *(s32*)(spac + 0x6C) = *(s32*)(spac + 0x12C);
                                        if (func_0203d5bc((s32*)(spac + 0x6C)) < 0x40000) {
                                            SELF_I(self, 0xf8) = j;
                                            SELF_SB(self, 0x4c + ang) = 1;
                                            SELF_SB(self, 0x6c + ang) = 1;
                                            SELF_SB(self, 0xac + ang) = 1;
                                            *(s32*)(spac + 0x1C) = 1;
                                        }
                                        *(s32*)(spac + 0x40) = (s16)(*(s32*)(spac + 0x40) + 0x800);
                                        ang += 1;
                                    } while (ang < 0x20);
                                }
                            }
                        }
                    }
                }
                j += 1;
            } while (j < MGR_I(self, 0x4668));
        }
    }
    *(s32*)(spac + 0x7C) = *(s32*)(spac + 0x48);
    *(s32*)(spac + 0x80) = *(s32*)(spac + 0x48);
    ang = *(s32*)(spac + 0x48);
    idx = *(s32*)(spac + 0x48);
    do {
        if (SELF_SB(self, 0x4c + idx) == 1) {
            *(s32*)(spac + 0x84) = *(s32*)(spac + 0x4C);
            *(s32*)(spac + 0x88) = *(s32*)(spac + 0x50);
            func_0203d388((s32*)(spac + 0x84), ang);
            *(s32*)(spac + 0x7C) += *(s32*)(spac + 0x84);
            *(s32*)(spac + 0x80) += *(s32*)(spac + 0x88);
        }
        ang = (s16)(ang + 0x800);
        idx += 1;
    } while (idx < 0x20);
    func_0203d434((s32*)(spac + 0x7C));
    func_0203d630((s32*)(spac + 0x7C), *(s32*)(spac + 0x50));
    SELF_I(self, 0x8) -= *(s32*)(spac + 0x7C);
    SELF_I(self, 0xc) -= *(s32*)(spac + 0x80);
    func_0203d6d0((s32*)(spac + 0x134), (int *)(self + 0x8), (int *)(self + 0x10));
    *(s32*)(spac + 0x24) += 0x80;
    *(s32*)(spac + 0x20) += 1;
    *(s32*)(spac + 0x100) = func_0203d614((s32*)(spac + 0x134));
    if (*(s32*)(spac + 0x1C) == 1 && *(s32*)(spac + 0x20) < 0x21 && *(s32*)(spac + 0x24) <= *(s32*)(spac + 0x100))
        goto loop_15;
    }
    *(s32*)(spac + 0x8C) = 0;
    *(s32*)(spac + 0x90) = 0;
    if (*(s32*)(spac + 0xC) == 1) {
        s32 v;
        s32 i;
        v = 0;
        for (i = 0; i < 0x20; i++) {
            if (SELF_SB(self, 0x8c + i) == 1) {
                *(s32*)(spac + 0x94) = 0;
                *(s32*)(spac + 0x98) = 0x1000;
                func_0203d388((s32*)(spac + 0x94), ang_idx(v));
                *(s32*)(spac + 0x8C) -= *(s32*)(spac + 0x94);
                *(s32*)(spac + 0x90) -= *(s32*)(spac + 0x98);
            }
            v += 0x10000;
        }
    } else {
        s32 v;
        s32 i;
        v = 0;
        for (i = 0; i < 0x20; i++) {
            if (SELF_SB(self, 0x6c + i) == 1 && SELF_SB(self, 0xac + i) == 0) {
                *(s32*)(spac + 0x94) = 0;
                *(s32*)(spac + 0x98) = 0x1000;
                func_0203d388((s32*)(spac + 0x94), ang_idx(v));
                *(s32*)(spac + 0x8C) -= *(s32*)(spac + 0x94);
                *(s32*)(spac + 0x90) -= *(s32*)(spac + 0x98);
            }
            v += 0x10000;
        }
    }
    if (*(s32*)(spac + 0x8C) != 0 || *(s32*)(spac + 0x90) != 0) {
        s32 scale;
        s32 t;
        s32 u;
        func_0203d480((s32*)(spac + 0x13C), (s32*)(spac + 0x8C));
        t = FIXMUL(SELF_I(self, 0x20), *(s32*)(spac + 0x8C)) + FIXMUL(SELF_I(self, 0x24), *(s32*)(spac + 0x90));
        if (*(s32*)(spac + 0x18) == 1) {
            scale = FIXMUL(t, 0x1800);
        } else if (*(s32*)(spac + 0x4) == 1) {
            scale = FIXMUL(t, 0x1400);
            u = (RAND15() << 4 >> 15) << 12 >> 6;
            scale += FIXMUL(scale, u);
            func_0203d388((s32*)(spac + 0x8C), (s16)((RAND15() << 12 >> 15) - 0x800));
        } else if (*(s32*)(spac + 0x10) == 1) {
            SELF_I(self, 0x20) = 0;
            SELF_I(self, 0x24) = 0;
            u = (RAND15() << 4 >> 15) << 12 >> 5;
            scale = FIXMUL(u, -0x2000) - 0x2000;
            func_0203d388((s32*)(spac + 0x8C), (s16)((RAND15() << 12 >> 15) - 0x800));
        } else if (*(s32*)(spac + 0xC) == 1) {
            scale = FIXMUL(t, 0x1a00);
            u = (RAND15() << 4 >> 15) << 12 >> 6;
            scale += FIXMUL(scale, u);
            if (SELF_I(self, 0x8) < 0x18000 && *(s32*)(spac + 0x90) > 0) {
                *(s32*)(spac + 0x8C) = 0;
                *(s32*)(spac + 0x90) = 0x1000;
            }
        } else {
            scale = FIXMUL(t, 0x1200);
        }
        SELF_I(self, 0x20) -= FIXMUL(scale, *(s32*)(spac + 0x8C));
        SELF_I(self, 0x24) -= FIXMUL(scale, *(s32*)(spac + 0x90));
    }
    if (*(s32*)(spac + 0x18) == 1) {
        *(s32*)(spac + 0xF0) = SELF_I(self, 0x8);
        if (*(s32*)(spac + 0xF0) < 0xd0000)
            return;
        if (*(s32*)(spac + 0xF0) < 0xd8000) {
            SELF_I(self, 0x8) = 0xd8000;
            return;
        }
        if (*(s32*)(spac + 0xF0) < 0xe8000)
            SELF_I(self, 0x8) = 0xe8000;
        func_ov006_0211470c((s32*)(spac + 0x144), (int *)MGR_P(self, 0x4684));
        SELF_I(self, 0xc) = *(s32*)(spac + 0x148) - 0x20001 - SELF_I(self, 0x28);
        func_ov006_021146f4((s32*)(spac + 0x14C), MGR(self) + 0x4684);
        if (*(s32*)(spac + 0x150) >= -0x1000)
            return;
        func_ov006_021146f4((s32*)(spac + 0x154), MGR(self) + 0x4684);
        if (SELF_I(self, 0x24) < *(s32*)(spac + 0x158))
            return;
        SELF_I(self, 0x20) = 0;
        func_ov006_021146f4((s32*)(spac + 0x15C), MGR(self) + 0x4684);
        SELF_I(self, 0x24) = *(s32*)(spac + 0x160);
        *(s32*)(spac + 0xF0) = SELF_I(self, 0x24);
        if (*(s32*)(spac + 0xF0) < -0x6000) {
            *(s32*)(spac + 0xF4) = *(s32*)(spac + 0xF0) + 0x6000;
            SELF_I(self, 0x24) = ((*(s32*)(spac + 0xF4) + ((unsigned)*(s32*)(spac + 0xF4) >> 1 >> 30)) >> 2) - 0x6000;
        }
        if (SELF_I(self, 0x124) == 0) {
            *(s32*)(spac + 0xF0) = SELF_I(self, 0x24);
            if (*(s32*)(spac + 0xF0) < -0x6000) {
                func_02012718(0x16d, SELF_I(self, 0x8));
                func_02012718(0x16e, SELF_I(self, 0x8));
            } else {
                *(s32*)(spac + 0xF4) = -*(s32*)(spac + 0xF0);
                *(s32*)(spac + 0xF8) = *(s32*)(spac + 0xF4) << 7;
                *(s32*)(spac + 0xFC) = (*(s32*)(spac + 0xF8) >> 31) + ov006_mul_high(0x2aaaaaab, *(s32*)(spac + 0xF8));
                if (*(s32*)(spac + 0xFC) > 0x7f) *(s32*)(spac + 0xFC) = 0x7f;
                if (*(s32*)(spac + 0xFC) < 0) *(s32*)(spac + 0xFC) = 0;
                ov006_fall_sfx(self, *(s32*)(spac + 0xFC));
            }
            SELF_I(self, 0x124) = 5;
        }
        if (SELF_B(self, 0x128) != 0)
            return;
        if (SELF_I(self, 0x24) < -0x6000)
            func_02012718(0x16e, SELF_I(self, 0x8));
        SELF_B(self, 0x128) = 1;
        return;
    }
    if (*(s32*)(spac + 0x4) == 1) {
        *(s32*)(spac + 0x164) = *(s32*)(spac + 0x5C); *(s32*)(spac + 0x168) = *(s32*)(spac + 0x60);
        func_ov006_02111b90(self, 0x171, (s32*)(spac + 0x164));
    } else if (*(s32*)(spac + 0x8) == 1) {
        *(s32*)(spac + 0x16C) = *(s32*)(spac + 0x5C); *(s32*)(spac + 0x170) = *(s32*)(spac + 0x60);
        func_ov006_02111b90(self, 0x172, (s32*)(spac + 0x16C));
    } else if (*(s32*)(spac + 0xC) == 1) {
        *(s32*)(spac + 0x174) = *(s32*)(spac + 0x5C); *(s32*)(spac + 0x178) = *(s32*)(spac + 0x60);
        func_ov006_02111b90(self, 0x16f, (s32*)(spac + 0x174));
    } else if (*(s32*)(spac + 0x14) == 1) {
        *(s32*)(spac + 0x17C) = *(s32*)(spac + 0x5C); *(s32*)(spac + 0x180) = *(s32*)(spac + 0x60);
        func_ov006_02111b90(self, 0x1a3, (s32*)(spac + 0x17C));
    }
    *(s32*)(spac + 0x184) = SELF_I(self, 0x28) + 0x10000;
    *(s32*)(spac + 0x188) = 0;
    *(s64*)(spac + 0x108) = func_0203d5bc((s32*)(spac + 0x184));
    if (MGR_I(self, 0x466c) <= 0)
        return;
        for (j = 0; j < MGR_I(self, 0x466c); j++) {
            *(s32*)(spac + 0x110) = (s32)((j >= MGR_I(self, 0x466c)) ? 0 : MGR_P(self, 0x4720 + j * 4));
            if (func_ov006_021115c4((unsigned char *)*(s32*)(spac + 0x110)) == 0) {
                *(s32*)(spac + 0x9C) = SELF_I(self, 0x8);
                *(s32*)(spac + 0xA0) = SELF_I(self, 0xc);
                *(s32*)(spac + 0x110) = (s32)((j >= MGR_I(self, 0x466c)) ? 0 : MGR_P(self, 0x4720 + j * 4));
                func_ov006_0211470c((s32*)(spac + 0x184), (int *)*(s32*)(spac + 0x110));
                *(s32*)(spac + 0xA0) -= *(s32*)(spac + 0x188);
                *(s32*)(spac + 0x9C) -= *(s32*)(spac + 0x184);
                *(s32*)(spac + 0xA0) *= 2;
                if (func_0203d5bc((s32*)(spac + 0x9C)) < *(s64*)(spac + 0x108)) {
                    *(s32*)(spac + 0xF4) = SELF_I(self, 0x28);
                    *(s32*)(spac + 0xF8) = (*(s32*)(spac + 0xF4) + 0x10000) - func_0203d614((s32*)(spac + 0x9C));
                    if (*(s32*)(spac + 0xF4) < *(s32*)(spac + 0xF8)) {
                        *(s32*)(spac + 0x110) = (s32)((j >= MGR_I(self, 0x466c)) ? 0 : MGR_P(self, 0x4720 + j * 4));
                        func_ov006_0211470c((s32*)(spac + 0x18C), (int *)*(s32*)(spac + 0x110));
                        SELF_I(self, 0x44) = *(s32*)(spac + 0x18C);
                        SELF_I(self, 0x48) = *(s32*)(spac + 0x190);
                        *(s32*)(spac + 0x110) = (s32)((j >= MGR_I(self, 0x466c)) ? 0 : MGR_P(self, 0x4720 + j * 4));
                        func_ov006_021115cc((char *)*(s32*)(spac + 0x110));
                        func_ov006_02111dcc(self, 0x80);
                        SELF_B(self, 0x3a) = 1;
                        return;
                    }
                    *(s32*)(spac + 0xA4) = *(s32*)(spac + 0x9C);
                    *(s32*)(spac + 0xA8) = *(s32*)(spac + 0xA0);
                    func_0203d434((s32*)(spac + 0xA4));
                    *(s32*)(spac + 0xFC) = FIXMUL(*(s32*)(spac + 0xA4), *(s32*)(spac + 0xF8));
                    *(s32*)(spac + 0xF4) = FIXMUL(*(s32*)(spac + 0xA8), *(s32*)(spac + 0xF8));
                    *(s32*)(spac + 0xF0) = (*(s32*)(spac + 0xFC) + ((unsigned)*(s32*)(spac + 0xFC) >> 5 >> 26)) >> 6;
                    *(s32*)(spac + 0xA8) = (*(s32*)(spac + 0xF4) + ((unsigned)*(s32*)(spac + 0xF4) >> 5 >> 26)) >> 6;
                    SELF_I(self, 0x20) -= *(s32*)(spac + 0xF0);
                    SELF_I(self, 0x24) -= *(s32*)(spac + 0xA8);
                    return;
                }
            }
        }
}
