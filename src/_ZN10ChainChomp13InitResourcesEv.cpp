//cpp
extern "C" void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *bmd, int a, int b);
extern "C" void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern "C" void _ZN11ShadowModel12InitCylinderEv(void *self);
extern "C" void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, void *actor, void *pos, int fix, int t, unsigned int a, unsigned int b);
extern "C" void func_ov014_02111ebc(void *c, int i);
extern "C" void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, void *pos, void *v16, int e, int f);

extern char data_ov014_02114968;
extern char data_ov014_02114978;
extern char data_ov014_02114980;
extern char data_ov014_02114970;
extern int data_ov014_02114700[];

extern "C" int _ZN10ChainChomp13InitResourcesEv(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    void *f;
        int i;
    unsigned char *p;
    void *spawned;
    int *px;
    int *py;
    int *pz;
    int one;


    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov014_02114968);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x150, f, 1, 1);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov014_02114978);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov014_02114980);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov014_02114970);

    p = c + 0x1dc;
    i = 0;
    do {
        _ZN9ModelBase7SetFileEP8BMD_Fileii(p, *(void **)((char *)&data_ov014_02114978 + 4), 1, 1);
        i = i + 1;
        p = p + 0x50;
    } while (i < 7);

    _ZN11ShadowModel12InitCylinderEv(c + 0x1b4);
    {
    int si; unsigned char *sp;
    si = 0; sp = c + 0x40c;
    do {
        _ZN11ShadowModel12InitCylinderEv(sp);
        si = si + 1;
        sp = sp + 0x28;
    } while (si < 7);
    }

    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;

    {
        int v[3];
        v[0] = data_ov014_02114700[0];
        v[1] = data_ov014_02114700[1];
        v[2] = data_ov014_02114700[2];
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
            c + 0x110, c, v, 0x96000, 0x12c000, 0x200004, 0x26ff0);
    }

    func_ov014_02111ebc(c, 1);

        {
        int cnt = 0;
        unsigned char *dst = c;
        do {
            *(int *)(dst + 0x524) = *(int *)(c + 0x5c);
            cnt = cnt + 1;
            *(int *)(dst + 0x528) = *(int *)(c + 0x60);
            *(int *)(dst + 0x52c) = *(int *)(c + 0x64);
            dst = dst + 0xc;
        } while (cnt < 7);
    }


    *(int *)(c + 0x5ec) = *(int *)(c + 0x5c);
    *(int *)(c + 0x5f0) = *(int *)(c + 0x60);
    *(int *)(c + 0x5f4) = *(int *)(c + 0x64);

    spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0x1b, 0x11, c + 0x5c, 0, *(signed char *)(c + 0xcc), -1);
    *(int *)(c + 0x608) = *(int *)((unsigned char *)spawned + 4);
    one = 1;
    *(unsigned char *)((unsigned char *)spawned + 0x320) = (unsigned char)one;
    *(int *)(c + 0x60c) = 0;

    px = (int *)(((long long)(int)(c + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
    *px = *px + 0xc8000;
    py = (int *)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);
    *py = *py + 0xc8000;
    pz = (int *)(((long long)(int)(c + 0x64)) & 0xFFFFFFFFFFFFFFFFLL);
    *pz = *pz + 0xc8000;

    *(int *)(c + 0x5f8) = 0x50000;
    *(int *)(c + 0x80) = 0x1000;
    *(int *)(c + 0x84) = 0x1000;
    *(int *)(c + 0x88) = 0x1000;
    return one;
}
