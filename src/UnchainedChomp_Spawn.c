extern void *_ZN9ActorBasenwEj(unsigned int);
extern void func_020aed98(void);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void func_020733a8(void *p, int n, int sz, void *ctor, void *dtor);
extern int _ZTV14UnchainedChomp;
extern void _ZN5ModelC1Ev(void);
extern void _ZN5ModelD1Ev(void);
extern void _ZN11ShadowModelD1Ev(void);
extern void func_020072c0(void);
extern void func_0203d384(void);
extern void func_02011508(void);
extern void func_0203d73c(void);

void *UnchainedChomp_Spawn(void)
{
    char *p = (char *)_ZN9ActorBasenwEj(0x7a4);
    if (p) {
        func_020aed98();
        *(int *)p = (int)&_ZTV14UnchainedChomp;
        _ZN25MovingCylinderClsnWithPosC1Ev(p + 0x110);
        _ZN12WithMeshClsnC1Ev(p + 0x150);
        _ZN9ModelAnimC1Ev(p + 0x30c);
        func_020733a8(p + 0x370, 6, 0x50, (void *)_ZN5ModelC1Ev, (void *)_ZN5ModelD1Ev);
        func_020733a8(p + 0x550, 6, 0x28, (void *)_ZN11ShadowModelC1Ev, (void *)_ZN11ShadowModelD1Ev);
        _ZN11ShadowModelC1Ev(p + 0x640);
        func_020733a8(p + 0x6d8, 6, 0xc, (void *)func_0203d384, (void *)func_020072c0);
        func_020733a8(p + 0x720, 6, 0xc, (void *)func_0203d384, (void *)func_020072c0);
        func_020733a8(p + 0x768, 6, 6, (void *)func_0203d73c, (void *)func_02011508);
    }
    return p;
}
