//cpp
typedef struct { int a, b; } Pair2i;
typedef struct { int x, y, z; } Vector3;
typedef struct {
    void *tag;
    int f04, f08, f0c, f10, f14;
    unsigned short f18, f1a;
    int f1c, f20, f24;
} ClsnResultTmp;

extern "C" {
extern int _ZN6Player7IsInAirEv(void *self);
extern int _ZN6Player7IsStateERNS_5StateE(void *self, void *st);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *st);
extern void _ZN11RaycastLineC1Ev(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self, void *a, void *b, void *act);
extern int _ZN11RaycastLine10DetectClsnEv(void *self);
extern void _ZN11RaycastLine10GetClsnPosEv(void *ret, void *self);
extern void func_02035414(void *bgch);
extern void func_02035428(void *bgch);
extern int func_02037e38(unsigned int *p);
extern unsigned _ZNK10ClsnResult9GetClsnIDEv(void *self);
extern void *_ZN5Actor10FindWithIDEj(unsigned id);
extern void _ZN10ClsnResultD1Ev(void *self);

extern char data_ov002_0211001c;
extern int data_02099368;
}

extern "C" int func_ov002_020cef84(char *self)
{
    char rl[0x78];
    Vector3 v1, v2, cp;
    ClsnResultTmp tmp;
    int lim;

    _ZN11RaycastLineC1Ev(rl);
    if (!_ZN6Player7IsInAirEv(self) || *(unsigned char *)(self + 0x706) != 0 ||
        *(int *)(self + 0xa8) <= 0 || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211001c) ||
        *(unsigned char *)(self + 0x703) != 0 || *(unsigned char *)(self + 0x708) != 0 ||
        *(unsigned char *)(self + 0x709) != 0) {
        _ZN11RaycastLineD1Ev(rl);
        return 0;
    }

    v1.x = *(int *)(self + 0x5c);
    int y1 = *(int *)(self + 0x60);
    v1.y = y1;
    v1.z = *(int *)(self + 0x64);
    v2.x = *(int *)(self + 0x5c);
    int y2 = *(int *)(self + 0x60);
    v2.y = y2;
    v2.z = *(int *)(self + 0x64);
    v1.y = y1 + 0x64000;
    v2.y = y2 + 0x96000;
    lim = 0x8c000;
    {
        int t = *(int *)(self + 0x358) != 0;
        if (t != false) {
            if (*(int *)(self + 8) == 2) {
                v2.y += 0x32000;
                lim = 0xb4000;
            }
        }
    }
    func_02035414(rl);
    if (*(unsigned char *)(self + 0x6fb) != 0)
        func_02035428(rl);
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &v1, &v2, self);
    if (_ZN11RaycastLine10DetectClsnEv(rl)) {
        _ZN11RaycastLine10GetClsnPosEv(&cp, rl);
        if (func_02037e38((unsigned int *)(rl + 0x14)) == 3) {
            int t2 = *(int *)(self + 0x358) != 0;
            if (t2 == false) {
                _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211001c);
                *(int *)(self + 0x60) = cp.y - 0x78000;
                _ZN11RaycastLineD1Ev(rl);
                return 1;
            }
        }
        {
            int d = cp.y - *(int *)(self + 0x60);
            if (d < 0) d = -d;
            if (d < lim) {
                void *a;
                int *dst = &tmp.f04;
                int w0, w1;
                *(int *)(self + 0xa8) = -0x1000;
                *(unsigned char *)(((long long)(int)(self + 0x6e9)) & 0xffffffffffffffffLL) |= 8;
                w0 = *(int *)(rl + 0x14);
                w1 = *(int *)(rl + 0x18);
                dst[0] = w1 ? w0 : w0;
                dst[1] = w1;
                dst[2] = *(int *)(rl + 0x1c);
                dst[3] = *(int *)(rl + 0x20);
                dst[4] = *(int *)(rl + 0x24);
                tmp.tag = &data_02099368;
                tmp.f18 = *(unsigned short *)(rl + 0x28);
                tmp.f1a = *(unsigned short *)(rl + 0x2a);
                tmp.f1c = *(int *)(rl + 0x2c);
                tmp.f20 = *(int *)(rl + 0x30);
                tmp.f24 = *(int *)(rl + 0x34);
                a = _ZN5Actor10FindWithIDEj(_ZNK10ClsnResult9GetClsnIDEv(&tmp));
                if (a)
                    (*(void (**)(void *, char *))(*(int *)a + 0x70))(a, self);
                _ZN10ClsnResultD1Ev(&tmp);
            }
        }
    }
    _ZN11RaycastLineD1Ev(rl);
    return 0;
}
