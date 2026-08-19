enum { false, true };

typedef struct {
    int x, y, z;
} Vector3i;

typedef struct {
    int a, b;
} Pair2i;

typedef struct {
    void *tag;
    int f04;
    int f08;
    int f0c;
    int f10;
    int f14;
    unsigned short f18;
    unsigned short f1a;
    int f1c;
    int f20;
    int f24;
} ClsnResultTmp;

extern void func_02037318(void *self);
extern void _ZN10dBgCh_Actr18StopDetectingWaterEv(void *self);
extern void _ZN10dBgCh_Actr19StartDetectingWaterEv(void *self);
extern void func_020357e0(void *self);
extern void func_02035800(void *self);
extern void func_02035770(void *self);
extern void func_02035784(void *self);
extern void func_020357a0(void *self);
extern int _ZN6Player7IsStateERNS_5StateE(void *thiz, void *st);
extern void func_020357c0(void *self);
extern void func_020355e8(void *self);
extern void func_020355fc(void *self);
extern void _ZN10dBgCh_Actr15ClearLimMovFlagEv(void *self);
extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(void *self);
extern void func_020355c8(void *self);
extern void func_020355b4(void *self);
extern void func_020383e4(void *self);
extern void func_ov002_020eed24(void *c, void *arg);
extern void _ZN10dBgCh_Actr15ClearGroundFlagEv(void *self);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *self);
extern int func_02035638(void *p);
extern void func_020371fc(void *self);
extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
extern void func_ov002_020c16ec(void *o, void *s);
extern void _ZN5dBgPiD1Ev(void *self);
extern int func_02037e58(void *p);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *self, void *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern int func_020356c8(void *p);
extern void func_ov002_020cef84(void *c);
extern void *_ZNK10dBgCh_Actr13GetWallResultEv(void *self);
extern int func_ov002_020d0580(void *p);
extern int func_ov002_020c2138(void *self);

extern char data_ov002_02110514;
extern char data_ov002_021100f4;
extern char data_ov002_021102ec;
extern int data_02099368;

int func_ov002_020c25a8(void *arg0, int arg1)
{
    char *c = (char*)arg0;
    int ret;
    Vector3i v;
    Vector3i wn;
    ClsnResultTmp tmp;
    char *fr;
    int *dst;

    func_02037318(c + 0x380);
    _ZN10dBgCh_Actr18StopDetectingWaterEv(c + 0x380);

    {
        int b1;
        b1 = *(int*)(c + 0x354);
        b1 = (b1 != 0);
        if (b1 || *(unsigned char*)(c + 0x6ec) || *(unsigned char*)(c + 0x6fd))
            _ZN10dBgCh_Actr19StartDetectingWaterEv(c + 0x380);
    }

    func_020357e0(c + 0x380);

    if (*(unsigned char*)(c + 0x6fb))
        func_02035800(c + 0x380);

    func_02035770(c + 0x380);

    if (*(unsigned char*)(c + 0x703))
        func_02035784(c + 0x380);

    func_020357a0(c + 0x380);

    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110514))
        func_020357c0(c + 0x380);

    func_020355e8(c + 0x380);

    {
        int b2;
        b2 = *(int*)(c + 0x354);
        b2 = (b2 != 0);
        if (b2)
            func_020355fc(c + 0x380);
    }

    _ZN10dBgCh_Actr15ClearLimMovFlagEv(c + 0x380);

    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021100f4) && *(int*)(c + 0xa8) > 0)
        _ZN10dBgCh_Actr13SetLimMovFlagEv(c + 0x380);

    func_020355c8(c + 0x380);

    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102ec))
        func_020355b4(c + 0x380);

    func_020383e4(c + 0x380);

    if (*(unsigned char*)(c + 0x703))
        func_ov002_020eed24(c + 0x380, c);

    if (*(int*)(c + 0xa8) > 0x1e000)
        _ZN10dBgCh_Actr15ClearGroundFlagEv(c + 0x380);

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x380))
        *(unsigned char*)(c + 0x6e9) |= 1;

    if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x380))
        *(unsigned char*)(c + 0x6e9) |= 2;

    if (func_02035638(c + 0x380))
        *(unsigned char*)(c + 0x6e9) |= 4;

    if (*(unsigned char*)(c + 0x706) == 0)
        func_020371fc(c + 0x380);

    if (*(unsigned char*)(c + 0x6e9) & 1) {
        fr = (char*)_ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x380);
        dst = &tmp.f04;

        *(Pair2i*)dst = *(Pair2i*)(fr + 0x4);
        dst[2] = *(int*)(fr + 0xc);
        dst[3] = *(int*)(fr + 0x10);
        dst[4] = *(int*)(fr + 0x14);
        tmp.tag = &data_02099368;
        tmp.f18 = *(unsigned short*)(fr + 0x18);
        tmp.f1a = *(unsigned short*)(fr + 0x1a);
        tmp.f1c = *(int*)(fr + 0x1c);
        tmp.f20 = *(int*)(fr + 0x20);
        tmp.f24 = *(int*)(fr + 0x24);

        func_ov002_020c16ec(c, &tmp);
        _ZN5dBgPiD1Ev(&tmp);

        *(int*)(c + 0x644) = *(int*)(c + 0x60);
        ret = func_02037e58(fr + 4);

        if (*(unsigned char*)(c + 0x706) == 0 && ret != 1) {
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr + 4, &v);
            if (v.y != 0) {
                int t = _ZN4cstd4fdivEii(
                    (int)(((long long)v.x * *(int*)(c + 0xa4) + 0x800) >> 12) +
                    (int)(((long long)v.z * *(int*)(c + 0xac) + 0x800) >> 12),
                    v.y);
                t = -(t + 0x8000);
                if (t < 0)
                    *(int*)(c + 0xa8) = t;
            }
        }
        *(unsigned char*)(c + 0x6de) = 0;
        *(unsigned char*)(c + 0x712) = 0;
    } else {
        if (func_020356c8(c + 0x380))
            *(int*)(c + 0x684) = *(int*)(c + 0x60);
        *(unsigned char*)(c + 0x6de) = 1;
        *(unsigned char*)(c + 0x6df) = 0;
        if (func_02035638(c + 0x380) && *(int*)(c + 0xa8) > 0)
            *(int*)(c + 0xa8) = -0x1000;
        func_ov002_020cef84(c);
    }

    if (arg1 != 0) {
        *(unsigned char*)(c + 0x6e9) |= 1;
        *(unsigned char*)(c + 0x6de) = 0;
        *(unsigned char*)(c + 0x712) = 0;
    }

    {
        int wallFlag = *(unsigned char*)(c + 0x6e9) & 2;
        if (wallFlag == 0)
            return wallFlag;
    }

    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr13GetWallResultEv(c + 0x380) + 4, &wn);
    *(int*)(c + 0x560) = wn.x;
    *(int*)(c + 0x564) = wn.y;
    *(int*)(c + 0x568) = wn.z;

    if (*(unsigned char*)(c + 0x706) == 0) {
        *(int*)(c + 0x5c) -= *(int*)(c + 0x560) * 2;
        *(int*)(c + 0x64) -= *(int*)(c + 0x568) * 2;
    }

    ret = func_ov002_020d0580(c);
    if (ret != 0)
        return ret;

    ret = func_ov002_020c2138(c);
    if (ret != 0)
        return ret;
}
