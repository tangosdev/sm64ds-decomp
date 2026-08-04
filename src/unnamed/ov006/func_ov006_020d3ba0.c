//cpp
// NONMATCHING: register allocation (div=9). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;

struct Obj {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34(); virtual void v35();
    virtual int m_90();
};

extern "C" {
int RandomIntInternal(int *seed);
char *func_02054efc(void);
void MultiStore16(unsigned short val, char *dst, int nbytes);
void func_ov006_020d36a4(void *sb);
void func_ov006_020d122c(void *sb, int v);
void func_ov004_020b04d0(int v);
int func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);
void func_ov006_020d3668(void *sb);
void _ZN3G2x13SetBlendAlphaEPVttttt(void *reg, unsigned short a, unsigned short b, unsigned short c, unsigned short d);
void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);

int data_0209d4b8;
char data_ov006_0212e1a8[];
char data_ov006_0212e1ac[];
char data_ov006_0212e1b0[];
char data_ov006_0212e1b4[];
char data_ov006_0212e1b8[];
}
namespace G2S { char *GetBG0CharPtr(void); }

typedef struct { int a; int b; } Pair;
typedef struct { s32 lo, hi; } P8;
extern Pair data_ov006_0213b8b8[11];

#pragma opt_strength_reduction off

#define VCALL90(p) (((Obj *)(p))->m_90())

extern "C" void func_ov006_020d3ba0(char *sb)
{
    int r5;
    int r4;
    Pair sp3C[11];
    int sp10[11];

    *(u8 *)(sb + 0x46d4) = 0;
    *(u8 *)(sb + 0x46d5) = 0;

    if (*(s32 *)(sb + 0x53e8) > 0x270f) {
        *(s32 *)(sb + 0x53e8) = 0x270f;
    }
    r4 = 0;

    r5 = *(s32 *)(sb + 0xbc);
    if (VCALL90(sb) != 0) {
        if ((u32) *(s32 *)(sb + 0xbc) >= 7) {
            r5 = 7;
            r4 = 1;
        }
    } else if (r5 >= 0xa) {
        r4 = 1;
        r5 = (((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5) >> 15) + 4;
        if (r5 == *(s32 *)(sb + 0x53d4)) {
            r5 = 9;
        }
    }
    *(s32 *)(sb + 0x53d4) = r5;

    if (VCALL90(sb) != 0) {
        *(s32 *)(sb + 0x46c8) = 4;
    } else {
        *(s32 *)(sb + 0x46c8) = *(int *)(data_ov006_0212e1a8 + *(s32 *)(sb + 0x53d4) * 0x1c);
    }

    *(s32 *)(sb + 0x5368) = ((*(int *)(data_ov006_0212e1b0 + *(s32 *)(sb + 0x53d4) * 0x1c) - 1) * 5) + 0xb;

    {
        u32 t = *(u32 *)(sb + 0xbc);
        if (t >= 0x13) {
            int *p5368 = (int *)(((int)sb + 0x5368) & 0xFFFFFFFFFFFFFFFFLL);
            *p5368 = (int)((t / 10 - 1) * 5) + *p5368;
            if (*(s32 *)(sb + 0x5368) > 0x64) {
                *(s32 *)(sb + 0x5368) = 0x64;
            }
        }
    }

    {
        char *d;
        volatile u16 v1;
        volatile u16 v2;
        d = func_02054efc();
        v1 = 0;
        MultiStore16(v1, d, 0x6000);
        d = G2S::GetBG0CharPtr();
        v2 = 0;
        MultiStore16(v2, d, 0x6000);
    }

    *(s32 *)(sb + 0x46d8) = -1;
    *(s32 *)(sb + 0x46dc) = -1;
    *(s32 *)(sb + 0x46e0) = -1;
    *(s32 *)(sb + 0x46e4) = -1;
    *(s32 *)(sb + 0x46f0) = -1;
    *(s32 *)(sb + 0x46f4) = -1;
    *(s32 *)(sb + 0x46e8) = -1;
    *(s32 *)(sb + 0x46ec) = -1;

    {
        int z = 0;
        *(u8 *)(sb + 0x4704) = z;
        *(u8 *)(sb + 0x4705) = z;
        *(u8 *)(sb + 0x4708) = z;
        *(u8 *)(sb + 0x4709) = z;
        *(u8 *)(sb + 0x53c8) = z;
        *(s32 *)(sb + 0x53cc) = z;
        do {
            ((s32 *)(sb + 0x4694))[z] = z;
            z += 1;
        } while (z < 4);
    }

    *(s32 *)(sb + 0x5374) = 0;
    func_ov006_020d36a4(sb);
    func_ov006_020d122c(sb, *(int *)(data_ov006_0212e1ac + *(s32 *)(sb + 0x53d4) * 0x1c));
    func_ov004_020b04d0(0x20);

    *(u8 *)(sb + 0x4707) = 1;
    *(u8 *)(sb + 0x470a) = 1;
    {
        int i, j, off;
        for (i = 0, off = 0; i < 0x100; i++, off += 0x158) {
            for (j = 0; j < 0x158; j++) {
                *(*(u8 **)(sb + 0x470c) + off + j) = 0;
                *(*(u8 **)(sb + 0x4710) + off + j) = 0;
            }
        }
    }

    if (VCALL90(sb) != 0) {
        func_ov004_020ae5c4(sb, 0x20, -0xb4, 0x20, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
        func_ov004_020ae5c4(sb, 0x60, -0xb4, 0x60, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
        func_ov004_020ae5c4(sb, 0xa0, -0xb4, 0xa0, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
        func_ov004_020ae5c4(sb, 0xe0, -0xb4, 0xe0, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
    } else {
        func_ov004_020ae5c4(sb, 0x20, -0xd4, 0x20, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
        func_ov004_020ae5c4(sb, 0x60, -0xd4, 0x60, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
        func_ov004_020ae5c4(sb, 0xa0, -0xd4, 0xa0, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
        func_ov004_020ae5c4(sb, 0xe0, -0xd4, 0xe0, *(s32 *)(sb + 0x4700), *(s32 *)(sb + 0x53e4), 1);
    }

    (*(u8 *)(((int)sb + 0x470a) & 0xFFFFFFFFFFFFFFFFLL))++;
    {
        typedef struct { Pair e[11]; } Blk88;
        *(Blk88 *)sp3C = *(Blk88 *)data_ov006_0213b8b8;
    }

    {
        int i;
        for (i = 0; i < 0xb; i++) {
            sp10[i] = 0;
        }
    }

    if (r4 == 1) {
        if (VCALL90(sb) != 0) {
            int msk = 0x7fff;
            int cnt = 3;
            do {
                int idx = ((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & msk) * 0xb) >> 15;
                if (sp10[idx] == 0) {
                    cnt--;
                    sp10[idx] = 1;
                }
            } while (cnt > 0);
        } else {
            int sel = *(int *)(data_ov006_0212e1b4 + *(s32 *)(sb + 0x53d4) * 0x1c);
            switch (sel) {
            case 0:
                break;
            case 1:
                sp10[((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5) >> 15] = 1;
                break;
            case 2: {
                int cnt = (((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3) >> 15) + 1;
                int j = 0;
                if (cnt > 0) {
                    int *seedp2 = &data_0209d4b8;
                    do {
                        sp10[((u32)(((u32)RandomIntInternal(seedp2) >> 16) & 0x7fff) * 0xb) >> 15] = 1;
                        j++;
                    } while (j < cnt);
                }
                break;
            }
            default:
                break;
            }
        }
    }

    {
        int sel2;
        if (VCALL90(sb) != 0) {
            switch (*(s32 *)(sb + 0x53d4)) {
            case 0:
            case 1:
            case 2:
                sel2 = 0;
                break;
            case 3:
            case 4:
                sel2 = 1;
                break;
            default:
                sel2 = 2;
                break;
            }
        } else {
            sel2 = *(int *)(data_ov006_0212e1b4 + *(s32 *)(sb + 0x53d4) * 0x1c);
        }

        switch (sel2) {
        case 0:
            break;
        case 1: {
            int i;
            for (i = 0; i < 5; i++) {
                if (sp10[i] == 0) {
                    int x = sp3C[i].a;
                    int y = sp3C[i].b - 0x20;
                    func_ov004_020ae5c4(sb, x, y, x + 0x40, y, *(s32 *)(sb + 0x53e4), 1);
                }
            }
            break;
        }
        case 2:
        case 3: {
            int i;
            for (i = 0; i < 0xb; i++) {
                if (sp10[i] == 0) {
                    int x = sp3C[i].a;
                    int y = sp3C[i].b - 0x20;
                    func_ov004_020ae5c4(sb, x, y, x + 0x40, y, *(s32 *)(sb + 0x53e4), 1);
                }
            }
            break;
        }
        default:
            break;
        }
    }

    (*(u8 *)(((unsigned int)sb + 0x470au) & 0xFFFFFFFFFFFFFFFFuLL))++;
    if (VCALL90(sb) == 0) {
        int v = *(int *)(data_ov006_0212e1b8 + *(s32 *)(sb + 0x53d4) * 0x1c);
        if (v != 0 && v == 1) {
            func_ov004_020ae5c4(sb, 0x60, 0x2d, 0xa0, 0x2d, *(s32 *)(sb + 0x53e4), 1);
            func_ov004_020ae5c4(sb, 0x20, 0x5a, 0x60, 0x5a, *(s32 *)(sb + 0x53e4), 1);
            func_ov004_020ae5c4(sb, 0xa0, 0x5a, 0xe0, 0x5a, *(s32 *)(sb + 0x53e4), 1);
        }
    }

    *(u8 *)(sb + 0x4707) = 0;
    (*(u8 *)(((long long)(int)(sb + 0x470a)) & 0xFFFFFFFFFFFFFFFFLL))++;
    {
        int i, j, off;
        for (i = 0, off = 0; i < 0x100; i++, off += 0x158) {
            for (j = 0; j < 0x158; j++) {
                *(*(u8 **)(sb + 0x470c) + off + j) = 0;
            }
        }
    }

    func_ov006_020d3668(sb);
    *(s32 *)(sb + 0x5370) = 1;
    *(volatile u16 *)0x04000050 = 0;
    _ZN3G2x13SetBlendAlphaEPVttttt((void *)0x04001050, 4, 8, 6, 0x10);
    func_ov004_020b0cac(0xd, 0x80, 0x60, 1, -1, 0xd);

    *(s32 *)(sb + 0x53c4) = 0x3c;
    *(s32 *)(sb + 0x46d0) = 1;
    {
        int i = 0;
        for (; i < 4; i++) {
            ((s32 *)(sb + 0x5378))[i] = 0;
            ((s32 *)(sb + 0x5388))[i] = 0;
            ((u8 *)(sb + 0x5398))[i] = 0;
            ((s32 *)(sb + 0x539c))[i] = 0;
            ((s32 *)(sb + 0x53ac))[i] = 0;
        }
        *(s16 *)(sb + 0x53bc) = 0;
        *(s32 *)(sb + 0x53d0) = 0;
        *(s32 *)(sb + 0x53d8) = 0;
        *(u8 *)(sb + 0x53dc) = 0;
        *(u8 *)(sb + 0x53dd) = 0;
        *(u8 *)(sb + 0x53de) = 0;
        {
            int z = 0;
            int x = 0x20;
            int w = 0;
            for (; z < 4; z++) {
                ((P8 *)(sb + 0x4724))[z].lo = x << 12;
                ((P8 *)(sb + 0x4724))[z].hi = 0xb0000;
                ((P8 *)(sb + 0x4744))[z].lo = w;
                ((P8 *)(sb + 0x4744))[z].hi = w;
                x += 0x40;
            }
            *(s32 *)(sb + 0x4764) = w;
            {
                int v = 0;
                for (; v < 0x80; v++) {
                    *(u8 *)(sb + 0x477c) = 0;
                    *(s32 *)(sb + 0x4778) = 0;
                    sb += 0x18;
                }
            }
        }
    }
}
