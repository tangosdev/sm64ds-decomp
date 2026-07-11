extern void func_ov004_020b0cac(int a0, int a1, int a2, int a3, int a4, short a5);
extern void func_02012790(int a);
extern void func_ov004_020b0aa0(int arg);
extern int func_0203d614(int *p);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_0203d388(int *p, int angle);
extern int func_0203d434(int *in);
extern void func_0203d630(int *p, int m);
extern void func_ov006_02125f68(char *c);
extern void func_0203d6d0(int *o, int *a, int *b);
extern void func_02012718(int a, int b);
extern void _Z14ApproachLinearRsss(short *a, short b, short cc);
extern void func_ov004_020adb1c(int self);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int a);
extern void func_ov004_020b67e8(int v);
extern void func_ov004_020b0a54(int c);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int cc, int d, int e, const void *f, void *g);
extern int *_ZN8Particle6System12FromUniqueIDEj(unsigned int a);
extern void func_ov006_0212a2e0(char *c);
extern void func_ov006_02125890(char *c);
extern void func_ov006_02126948(char *c);
extern int __aeabi_idiv(int a, int b);

extern unsigned char data_020a0e40[];
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];

#define AT(p,off) ((void*)(int)(((long long)(int)((char*)(p)+(off)))&0xFFFFFFFFFFFFFFFFLL))
#define LNDR(e) ((int)(((long long)(e))&0xFFFFFFFFFFFFFFFFLL))
#define C1 ((char*)(void*)(int)(((long long)(int)(c))&0xFFFFFFFFFFFFFFFFLL))
#define C2 ((char*)(void*)(int)(((long long)(int)(C1))&0xFFFFFFFFFFFFFFFFLL))
#define ATS(off) AT(C1,off)
#define ATI(off) AT(C2,off)
#define I(o) (*(int*)(c + (o)))
#define H(o) (*(short*)(c + (o)))
#define B(o) (*(unsigned char*)(c + (o)))
#define IA(o) (*(int*)AT(c,(o)))
#define HA(o) (*(short*)AT(c,(o)))
#define atan2 _ZN4cstd5atan2E5Fix12IiES1_
#define pnew _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE
#define pfromid _ZN8Particle6System12FromUniqueIDEj
#define ApproachLinear _Z14ApproachLinearRsss

struct SPS {
    int a[2];
    int b[2];
    int cc[3];
};

int func_ov006_021283a4(char *c) {
    int r5, r4;
    int idx, r3rec, r7, j;
    struct SPS s;
    int v, q, t;
    unsigned short m;
    int gate;
    int id2, id1;
    int *p;

    if (B(0xc4) == 0) {
        B(0xc3) = 1;
        B(0xc4) = 1;
        H(0xc0) = 0;
    }

    H(0xab7e) = H(0xab78);
    H(0xab80) = H(0xab7a);
    H(0xab82) = H(0xab7c);
    I(0xab40) = I(0xab38);
    I(0xab44) = I(0xab3c);

    r4 = (I(0xba08) << 16) - 0x1d0000;
    r5 = I(0xab6c);

    switch (I(0xb9f4)) {
    case 0:
        IA(0xb9fc) -= 1;
        v = I(0xb9fc);
        if (v == 0x3c) {
            I(0xb9f4) = 1;
            func_ov004_020b0cac(0xc, 0x80, 0x60, 0, -1, 0xd);
            func_02012790(0x2a);
        } else if (v == 0x78 || v == 0xb4 || v == 0xf0) {
            func_02012790(0x2b);
        }
        break;

    case 1:
        if (I(0xb9fc) > 0) {
            IA(0xb9fc) -= 1;
            if (I(0xb9fc) == 0)
                func_ov004_020b0aa0(0xc);
        }

        idx = data_020a0e40[0];
        r3rec = data_020a0de8[idx * 4];
        j = idx * 4;
        r7 = 0;
        if (r3rec != 0)
            r7 = (data_020a0de9[j] != 0);
        if (r7 != 0) {
            int vb = ((volatile unsigned char*)data_020a0deb)[(unsigned int)idx * 4];
            int va = ((volatile unsigned char*)data_020a0dea)[(unsigned int)idx * 4];
            I(0xab70) = va;
            I(0xab74) = vb;
        } else if (r3rec != 0) {
            s.a[0] = ((data_020a0dea[(unsigned int)idx * 4] - I(0xab70)) << 12) / 296;
            s.a[1] = ((data_020a0deb[(unsigned int)idx * 4] - I(0xab74)) << 12) / 296;
            I(0xab70) = data_020a0dea[(unsigned int)idx * 4];
            I(0xab74) = data_020a0deb[(unsigned int)idx * 4];
            if (func_0203d614(s.a) >= 0x400) {
                s.a[0] = 0;
                s.a[1] = 0;
            }
            r7 = atan2(I(0xab64), I(0xab60));
            func_0203d388(s.a, (short)-r7);
            s.a[1] = s.a[1] << 2;
            func_0203d388(s.a, r7);
            if (I(0xab64) >= 0) {
                if (s.a[1] > 0)
                    s.a[1] = 0;
            }
            if (s.a[1] > 0) {
                if (s.a[1] >= -I(0xab64)) {
                    I(0xab64) = 0;
                    s.a[1] = 0;
                }
            }
            IA(0xab60) += s.a[0];
            IA(0xab64) += s.a[1];
        }

        if (I(0xab60) != 0 || I(0xab64) != 0) {
            if (func_0203d614((int*)AT(c,0xab60)) >= 0x8000 && func_0203d434((int*)ATS(0xab60)) != 0)
                func_0203d630((int*)ATI(0xab60), 0x8000);
        }
        func_ov006_02125f68(c);

        IA(0xab38) += I(0xab60);
        IA(0xab3c) += I(0xab64);

        if (I(0xab3c) >= I(0xab44)) {
            IA(0xab58) += I(0xab3c) - I(0xab44);
        } else if (I(0xab58) > 0) {
            IA(0xab58) += I(0xab3c) - I(0xab44);
            if (I(0xab58) < 0) {
                IA(0xaba0) += (-I(0xab58)) / 128;
                I(0xab58) = 0;
            }
        } else {
            IA(0xaba0) += (I(0xab44) - I(0xab3c)) / 128;
        }

        if (I(0xaba0) >= 0x37000)
            I(0xaba0) = 0x37000;
        func_0203d6d0(s.b, (int*)AT(C1,0xab38), (int*)AT(c,0xab50));
        gate = (func_0203d614(s.b) >= 0x30000) ? 1 : 0;
        if ((int)(((long long)gate) & 0xFFFFFFFFFFFFFFFFLL) != 0) {
            I(0xab50) = I(0xab38);
            I(0xab54) = I(0xab3c);
            if (I(0xaba0) < 0x10000)
                func_02012718(0x165, I(0xab38));
            else if (I(0xaba0) < 0x20000)
                func_02012718(0x166, I(0xab38));
            else
                func_02012718(0x167, I(0xab38));
        }

        I(0xab6c) = I(0xab3c) - 0x190000;
        if (I(0xab6c) > r5)
            I(0xab6c) = r5;
        if (I(0xab6c) < 0)
            I(0xab6c) = 0;
        if (I(0xab6c) >= r4)
            I(0xab6c) = r4;

        if (func_0203d614((int*)AT(c,0xab60)) >= 0x10) {
            short *hp;
            v = func_0203d614((int*)ATS(0xab60));
            hp = (short*)AT(c,0xab78);
            q = __aeabi_idiv(v * 0x2710, I(0xaba0));
            *hp = *hp + (unsigned short)q;
            m = atan2(I(0xab60), I(0xab64));
            ApproachLinear((short*)AT(c,0xab7c), (short)m, (short)(func_0203d614((int*)ATI(0xab60)) / 16));
        }

        if (B(0xb9e5) == 1 || I(0xb9dc) <= 0) {
            I(0xb9f0) = 1;
            I(0xb9f4) = 4;
            break;
        }
        IA(0xb9dc) -= 1;
        IA(0xb9e0) += 1;
        t = I(0xb9dc);
        if (t % 60 == 0) {
            func_02012790(0x2b);
        } else {
            if (t < 0x258 && t % 30 == 0)
                func_02012790(0x2b);
            else if (t < 0xb4 && t % 15 == 0)
                func_02012790(0x2b);
        }
        if (I(0xab3c) - I(0xaba0) < (I(0xba04) << 12) + 0x10000) {
            func_ov004_020adb1c(I(0xb9e0));
            I(0xb9f4) = 2;
            _ZN5Sound12PlayBank2_2DEj(0x10e);
        }
        break;

    case 2:
        IA(0xba0c) += 1;
        if (I(0xba0c) >= 0x3c) {
            func_ov004_020b67e8(4);
            func_ov004_020b0a54(0x13);
            I(0xba10) = 0xb4;
            I(0xb9f4) = 3;
        }
        /* fall through */

    case 3:
        B(0xc3) = 0;
        if (I(0xba10) > 0)
            IA(0xba10) -= 1;
        idx = data_020a0e40[0];
        r3rec = data_020a0de8[idx * 4];
        j = idx * 4;
        r7 = 0;
        if (r3rec != 0)
            r7 = (data_020a0de9[j] != 0);
        if (r7 != 0)
            I(0xba10) = 0;
        func_ov006_02125f68(c);

        if (I(0xab60) != 0 || I(0xab64) != 0) {
            r7 = func_0203d614((int*)AT(c,0xab60));
            if (I(0xab3c) < 0xc0000)
                r7 -= 0x80;
            if (r7 < 0) {
                I(0xab60) = 0;
                I(0xab64) = 0;
            } else if (func_0203d434((int*)ATS(0xab60)) != 0) {
                func_0203d630((int*)ATI(0xab60), r7);
            }
        }

        IA(0xab38) += I(0xab60);
        IA(0xab3c) += I(0xab64);
        if (I(0xab3c) < 0xe8000)
            B(0xb9f8) = 0;
        else
            B(0xb9f8) = 1;

        if (func_0203d614((int*)AT(c,0xab60)) >= 0x10) {
            short *hp;
            v = func_0203d614((int*)ATS(0xab60));
            hp = (short*)AT(c,0xab78);
            q = __aeabi_idiv(v * 0x2710, I(0xaba0));
            *hp = *hp + (unsigned short)q;
            H(0xab7c) = atan2(I(0xab60), I(0xab64));
        }

        I(0xab6c) = I(0xab3c) - 0x190000;
        if (I(0xab6c) > r5)
            I(0xab6c) = r5;
        if (I(0xab6c) < 0)
            I(0xab6c) = 0;
        if (I(0xab6c) >= r4)
            I(0xab6c) = r4;
        break;

    case 4:
        if (I(0xba0c) < 0x3c) {
            if (I(0xab60) != 0 || I(0xab64) != 0) {
                r4 = func_0203d614((int*)AT(c,0xab60)) + 0x100;
                if (r4 >= 0x8000)
                    r4 = 0x8000;
                if (func_0203d434((int*)ATS(0xab60)) != 0)
                    func_0203d630((int*)ATI(0xab60), r4);
            }
            IA(0xba0c) += 1;
        } else if (I(0xba0c) == 0x3c) {
            if (I(0xaba0) > 0) {
                IA(0xaba0) -= 0x1000;
                if (I(0xaba0) <= 0) {
                    int t1, t0;
                    I(0xaba0) = 0;
                    t1 = I(0xab6c) - I(0xab3c) + 0x110000;
                    t0 = I(0xab38) - 0x80000;
                    s.cc[0] = t0;
                    s.cc[1] = t1;
                    s.cc[2] = 0x20000;
                    id1 = pnew(0, 0xf1, s.cc[0] << 3, s.cc[1] << 3, s.cc[2] << 3, 0, 0);
                    id2 = pnew(0, 0xf2, s.cc[0] << 3, s.cc[1] << 3, s.cc[2] << 3, 0, 0);
                    p = pfromid(id1);
                    if (p != 0) {
                        p[0x11] = 0xffff8222;
                        p[0x12] = 0x2e14;
                        p[0x13] = 0x29cf;
                        p[0x14] = 0xc5f;
                    }
                    p = pfromid(id2);
                    if (p != 0) {
                        p[0x12] = 0x4b17;
                        p[0x14] = 0x3999;
                    }
                    func_02012718(0x16b, I(0xab38));
                    IA(0xba0c) += 1;
                }
            }
        } else {
            IA(0xba0c) += 1;
            if (I(0xba0c) >= 0x5a) {
                func_ov004_020b67e8(0xe);
                func_ov004_020b0a54(3);
                I(0xb9f4) = 5;
            }
        }

        if (func_0203d614((int*)AT(c,0xab60)) >= 0x10) {
            short *hp;
            v = func_0203d614((int*)ATS(0xab60));
            hp = (short*)AT(c,0xab78);
            q = __aeabi_idiv(v * 0x2710, I(0xaba0));
            *hp = *hp + (unsigned short)q;
            H(0xab7c) = atan2(I(0xab60), I(0xab64));
        }
        break;

    case 5:
        B(0xc3) = 0;
        break;
    }

    func_ov006_0212a2e0(c);
    func_ov006_02125890(c);
    func_ov006_02126948(c);
    return 1;
}
