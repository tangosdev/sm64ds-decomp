typedef struct { int a, b; } Pair2i;

typedef struct {
    void *tag;
    int f04, f08, f0c, f10, f14;
    unsigned short f18, f1a;
    int f1c, f20, f24;
} ClsnResultTmp;

typedef struct { int x, y, z; } Vector3i;

extern void _ZN13RaycastGroundC1Ev(void *self);
extern int _ZN6Player12Unk_020c9e5cEh(void *self, unsigned char state);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, void *pos, void *act);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void func_ov002_020c16ec(void *o, void *s);
extern void _ZN10ClsnResultD1Ev(void *self);
extern void _ZN4BgCh19StartDetectingWaterEv(void *self);
extern void _ZN4BgCh19StartDetectingToxicEv(void *self);
extern void _ZN4BgCh21StopDetectingOrdinaryEv(void *self);
extern int func_02037e78(int *p);
extern int func_02037e20(int *p);
extern void func_ov002_020c133c(void *a);
extern void _ZN13RaycastGroundD1Ev(void *self);

extern int data_02099368;
extern int data_0209f32c;

void func_ov002_020c14b8(void *arg0)
{
    char *c = (char *)arg0;
    char rg[0x50];
    Vector3i pos;
    ClsnResultTmp tmp;
    int savedY;

    _ZN13RaycastGroundC1Ev(rg);

    savedY = 0x80000000;
    if (_ZN6Player12Unk_020c9e5cEh(c, 7) || _ZN6Player12Unk_020c9e5cEh(c, 0xa) ||
        _ZN6Player12Unk_020c9e5cEh(c, 0xb) || _ZN6Player12Unk_020c9e5cEh(c, 0xe))
        savedY = *(int *)(c + 0x60);
    *(int *)(c + 0x648) = 0x80000000;
    *(int *)(c + 0x644) = *(int *)(c + 0x648);
    *(int *)(c + 0x64c) = 0x80000000;
    {
        int t = *(int *)(c + 0x60) + 0xa0000;
        if (t >= *(int *)(c + 0x650))
            *(int *)(c + 0x650) = 0x80000000;
    }
    *(int *)(c + 0x668) = 0;
    *(int *)(c + 0x664) = *(int *)(c + 0x668);
    *(int *)(c + 0x658) = 0;
    {
        int z = *(int *)(c + 0x64);
        int y = *(int *)(c + 0x60) + 0xa0000;
        int x = *(int *)(c + 0x5c);
        pos.x = x;
        pos.y = y;
        pos.z = z;
    }
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, &pos, c);
    if (_ZN13RaycastGround10DetectClsnEv(rg)) {
        int *dst = &tmp.f04;
        *(Pair2i *)dst = *(Pair2i *)(rg + 0x14);
        dst[2] = *(int *)(rg + 0x1c);
        dst[3] = *(int *)(rg + 0x20);
        dst[4] = *(int *)(rg + 0x24);
        tmp.tag = &data_02099368;
        tmp.f18 = *(unsigned short *)(rg + 0x28);
        tmp.f1a = *(unsigned short *)(rg + 0x2a);
        tmp.f1c = *(int *)(rg + 0x2c);
        tmp.f20 = *(int *)(rg + 0x30);
        tmp.f24 = *(int *)(rg + 0x34);
        func_ov002_020c16ec(c, &tmp);
        _ZN10ClsnResultD1Ev(&tmp);
        *(int *)(c + 0x648) = *(int *)(rg + 0x44);
        *(int *)(c + 0x644) = *(int *)(c + 0x648);
        *(int *)(c + 0x668) = *(int *)(c + 0x664);
    }

    pos.y = *(int *)(c + 0x60) + 0x12c000;
    _ZN4BgCh19StartDetectingWaterEv(rg);
    _ZN4BgCh19StartDetectingToxicEv(rg);
    _ZN4BgCh21StopDetectingOrdinaryEv(rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, &pos, c);
    if (_ZN13RaycastGround10DetectClsnEv(rg)) {
        if (func_02037e78((int *)(rg + 0x14)) != 0) {
            *(int *)(c + 0x64c) = *(int *)(rg + 0x44);
            data_0209f32c = *(int *)(c + 0x64c);
        }
        if (func_02037e20((int *)(rg + 0x14)) != 0) {
            *(int *)(c + 0x650) = *(int *)(rg + 0x44);
        }
    }

    *(unsigned short *)(((long long)(int)(c + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
    *(unsigned short *)(((long long)(int)(c + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x10;
    func_ov002_020c133c(c);

    if (savedY != (int)0x80000000)
        *(int *)(c + 0x644) = savedY;

    _ZN13RaycastGroundD1Ev(rg);
}
