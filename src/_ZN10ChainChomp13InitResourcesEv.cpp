//cpp
// NONMATCHING: register allocation (div=38). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" void *Model_LoadFile(void *fp);
extern "C" void ModelBase_SetFile(void *self, void *bmd, int a, int b);
extern "C" void *Animation_LoadFile(void *fp);
extern "C" void ShadowModel_InitCylinder(void *self);
extern "C" void MovingCylinderClsnWithPos_Init(void *self, void *actor, void *pos, int fix, int t, unsigned int a, unsigned int b);
extern "C" void func_ov014_02111ebc(void *c, int i);
extern "C" void *Actor_Spawn(unsigned int a, unsigned int b, void *pos, void *v16, int e, int f);

extern void *data_ov014_02114968;
extern void *data_ov014_02114978;
extern void *data_ov014_02114980;
extern void *data_ov014_02114970;
extern int data_ov014_02114700[];

extern "C" int _ZN10ChainChomp13InitResourcesEv(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    void *f;
    int i;
    unsigned char *p;
    void *sp;
    int *q;

    f = Model_LoadFile(&data_ov014_02114968);
    ModelBase_SetFile(c + 0x150, f, 1, 1);
    Model_LoadFile(&data_ov014_02114978);
    Animation_LoadFile(&data_ov014_02114980);
    Animation_LoadFile(&data_ov014_02114970);

    p = c + 0x1dc;
    for (i = 0; i < 7; i++) {
        ModelBase_SetFile(p, (&data_ov014_02114978)[1], 1, 1);
        p += 0x50;
    }

    ShadowModel_InitCylinder(c + 0x1b4);
    p = c + 0x40c;
    for (i = 0; i < 7; i++) {
        ShadowModel_InitCylinder(p);
        p += 0x28;
    }

    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;

    {
        int v[3];
        v[0] = data_ov014_02114700[0];
        v[1] = data_ov014_02114700[1];
        v[2] = data_ov014_02114700[2];
        MovingCylinderClsnWithPos_Init(c + 0x110, c, v, 0x96000, 0x12c000, 0x200004, 0x26ff0);
    }

    func_ov014_02111ebc(c, 1);

    p = c;
    for (i = 0; i < 7; i++) {
        *(int *)(p + 0x524) = *(int *)(c + 0x5c);
        *(int *)(p + 0x528) = *(int *)(c + 0x60);
        *(int *)(p + 0x52c) = *(int *)(c + 0x64);
        p += 0xc;
    }

    *(int *)(c + 0x5ec) = *(int *)(c + 0x5c);
    *(int *)(c + 0x5f0) = *(int *)(c + 0x60);
    *(int *)(c + 0x5f4) = *(int *)(c + 0x64);

    sp = Actor_Spawn(0x1b, 0x11, c + 0x5c, 0, *(signed char *)(c + 0xcc), -1);
    *(int *)(c + 0x608) = *(int *)((unsigned char *)sp + 4);
    *(unsigned char *)((unsigned char *)sp + 0x320) = 1;
    *(int *)(c + 0x60c) = 0;

    q = (int *)(c + 0x5c);
    *q += 0xc8000;
    q = (int *)(c + 0x60);
    *q += 0xc8000;
    q = (int *)(c + 0x64);
    *q += 0xc8000;
    *(int *)(c + 0x5f8) = 0x50000;
    *(int *)(c + 0x80) = 0x1000;
    *(int *)(c + 0x84) = 0x1000;
    *(int *)(c + 0x88) = 0x1000;
    return 1;
}
