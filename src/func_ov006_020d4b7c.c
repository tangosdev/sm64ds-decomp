//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef long long s64;

extern "C" {
void func_ov006_020d3ba0(char *c);
void func_ov006_020d36a4(char *c);
void func_ov006_020d1a3c(char *c);
void func_ov006_020d1958(char *c);
void func_ov006_020d1ba0(char *c);
void func_ov006_020d27dc(char *c);
void FreeGfxSlotsById(int a);
void func_ov004_020adb1c(int a);
void func_ov004_020b0a54(int a);
int Vec2_Len(int *v);
int func_0203d434(int *v);
void func_0203d630(int *v, int m);
int RandomIntInternal(int *seed);
}

extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern int data_0209d4b8;
extern s16 data_02082214[];

struct VtObj {
    virtual void d00();
    virtual void d01();
    virtual void d02();
    virtual void d03();
    virtual void d04();
    virtual void d05();
    virtual void d06();
    virtual void d07();
    virtual void d08();
    virtual void d09();
    virtual void d10();
    virtual void d11();
    virtual void d12();
    virtual void d13();
    virtual void d14();
    virtual void d15();
    virtual void d16();
    virtual void d17();
    virtual void d18();
    virtual void d19();
    virtual void d20();
    virtual void d21();
    virtual void d22();
    virtual void d23();
    virtual void d24();
    virtual void d25();
    virtual void d26();
    virtual void d27();
    virtual void d28();
    virtual void d29();
    virtual void d30();
    virtual void d31();
    virtual void d32();
    virtual void d33();
    virtual void d34();
    virtual void d35();
    virtual int m36();
};

#define AT(p,off) ((void*)(int)(((long long)(int)((char*)(p)+(off)))&0xFFFFFFFFFFFFFFFFLL))
#define I(o)  (*(int*)(c+(o)))
#define B(o)  (*(u8*)(c+(o)))
#define H(o)  (*(short*)(c+(o)))
#define IA(o) (*(int*)AT(c,(o)))
#define IP(p,o) (*(int*)((char*)(p)+(o)))
#define BP(p,o) (*(u8*)((char*)(p)+(o)))
#define IAP(p,o) (*(int*)AT(p,(o)))
#define MULFX(a,b) ((int)(((s64)(a)*(b)+0x800)>>12))
#define VIRT() (((VtObj*)c)->m36())

#pragma opt_strength_reduction off

extern "C" int func_ov006_020d4b7c(char *c)
{
    int i2;
    char *p;
    int *vp;
    int t;
    int q;
    int k;
    int spd;
    int idx, j4, flag;
    int i1;
    int j;
    int i3;
    char *sc;
    int rnd1, rnd2;
    u32 bv, av;
    s16 cA;
    int idxA;
    char *e;
    int cosB, sinA;
    char *z;
    int *w;

    switch (I(0x46d0)) {
    case 0:
        if (B(0x46d4) == 1) {
            func_ov006_020d3ba0(c);
        } else if (B(0x46d5) == 1) {
            func_ov006_020d3ba0(c);
        } else {
            func_ov006_020d36a4(c);
        }
        I(0x46d0) = 1;
        // fall through
    case 1:
        func_ov006_020d1a3c(c);
        func_ov006_020d1958(c);
        func_ov006_020d1ba0(c);
        if (VIRT() != 0) {
            if (B(0x53dd) == 1) {
                IA(0x536c) += I(0x5374) * 5 + 0x20;
            }
        } else if (B(0x53dc) == 1) {
            IA(0x536c) += 0x64;
        } else {
            IA(0x536c) += I(0x5368);
        }
        spd = I(0x536c);
        IA(0x536c) &= 0xf;
        q = spd / 16;
        if (VIRT() == 0 || B(0x53dd) != 0) {
            if (I(0x53e0) > 0) {
                IA(0x53e0) -= 1;
                if (I(0x53e0) == 0) {
                    if (B(0x46d5) == 1) {
                        I(0x53c0) = 0x3c;
                        I(0x46d0) = 2;
                    } else if (I(0x5374) < 5) {
                        I(0x46d0) = 0;
                    } else {
                        I(0x53c0) = 0x3c;
                        I(0x46d0) = 2;
                        B(0x46d4) = 1;
                    }
                }
            }
            k = 0;
            if (q > 0) {
                do {
                    if (I(0x46d0) != 1)
                        break;
                    func_ov006_020d27dc(c);
                    k++;
                } while (k < q);
            }
        }
        if (I(0x53c4) > 0) {
            IA(0x53c4) -= 1;
            if (I(0x53c4) <= 0) {
                FreeGfxSlotsById(0xd);
                if (B(0xc4) == 0) {
                    B(0xc3) = 1;
                    B(0xc4) = 1;
                    H(0xc0) = 0;
                }
            }
        }
        func_ov004_020adb1c(I(0x53e8));
        I(0xb4) = I(0x53e8);
        break;
    case 2:
        B(0x53dc) = 0;
        if (VIRT() != 0) {
            if (I(0x53c0) > 0)
                IA(0x53c0) -= 1;
        } else {
            if (I(0x53c0) > 0)
                IA(0x53c0) -= 1;
        }
        if (I(0x53c0) != 0)
            break;
        B(0xc3) = 0;
        if (B(0x46d4) == 1) {
            I(0x46d0) = 3;
            func_ov004_020b0a54(0);
        } else {
            I(0x46d0) = 3;
            func_ov004_020b0a54(0x12);
            I(0x53d0) = 0xb4;
            I(0x4764) = 0;
        }
        break;
    case 3:
        if (I(0x53d0) > 0)
            IA(0x53d0) -= 1;
        flag = 0;
        idx = data_020a0e40[0];
        j4 = idx * 4;
        if (data_020a0de8[idx * 4] != 0) {
            if (data_020a0de9[j4] != 0)
                flag = 1;
        }
        if (flag != 0)
            I(0x53d0) = 0;
        if (VIRT() != 0)
            break;
        if (B(0x46d4) != 1)
            break;
        IA(0x4764) += 1;
        i1 = 0;
        do {
            char *w = c + i1 * 8;
            IAP(w, 0x4748) -= 0x100;
            IAP(w, 0x4724) += IP(w, 0x4744);
            IAP(w, 0x4728) += IAP(w, 0x4748);
            i1++;
        } while (i1 < 4);

        p = c;
        vp = (int *)AT(c, 0x4770);
        for (i2 = 0; i2 < 0x80; i2++) {
            if (BP(p, 0x477c) != 0) {
                IAP(p, 0x4768) += IP(p, 0x4770);
                IAP(p, 0x476c) += IP(p, 0x4774);
                t = Vec2_Len(vp) * 7 / 8;
                if (func_0203d434(vp) != 0)
                    func_0203d630(vp, t);
                *(int*)AT(p, 0x4778) = *(int*)AT(p, 0x4778) + 1;
                {
                    int* tb = (int*)(p + 0x4000);
                    if (tb[0x778/4] >= 0x10)
                        *(u8 *)(p + 0x477c) = 0;
                }
            }
            p += 0x18;
            vp += 6;
        }

        for (j = 0; j < 4; j++) {
            char *y = c + j * 4;
            if (IP(y, 0x4714) != 0) {
                i3 = 0;
                sc = c;
                do {
                    if (BP(sc, 0x477c) != 1) {
                        rnd1 = RandomIntInternal(&data_0209d4b8);
                        rnd2 = RandomIntInternal(&data_0209d4b8);
                        bv = ((u32)rnd2 >> 16) & 0x7fff;
                        av = ((u32)rnd1 >> 16) & 0x7fff;
                        cosB = data_02082214[((int)((bv << 15) >> 16) >> 4) * 2 + 1];
                        idxA = ((int)((av << 17) >> 16) >> 4) * 2;
                        cA = data_02082214[idxA + 1];
#define WP (c + i3 * 0x18)
                        {
                            IAP(WP, 0x4768) = MULFX(cosB, (int)((0x8000LL * cA + 0x800) >> 12));
                            sinA = data_02082214[idxA];
                            IAP(WP, 0x476c) = MULFX(cosB, (int)((0x8000LL * sinA + 0x800) >> 12));
                            z = c + j * 8;
                            IAP(WP, 0x4768) += IP(z, 0x4724);
                            IAP(WP, 0x476c) += IP(z, 0x4728);
                            IP(WP, 0x4770) = MULFX(cosB, (int)((0x1000LL * (int)(((s64)(int)cA) & 0xFFFFFFFFFFFFFFFFLL) + 0x800) >> 12));
                            IAP(WP, 0x4774) = MULFX(cosB, (int)((0x1000LL * sinA + 0x800) >> 12));
                        }
                        IAP(WP, 0x4774) -= 0x400;
                        BP(WP, 0x477c) = 1;
                        IP(WP, 0x4778) = 0;
                        break;
                    }
                    i3++;
                    sc += 0x18;
                } while (i3 < 0x80);
            }
        }
        break;
    }
    return 1;
}
