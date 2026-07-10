extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern void _ZN18MovingCylinderClsnC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern int VT0[];
int *SignPost_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1444);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)VT0;
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x320);
        _ZN11ShadowModelC1Ev((char *)p + 0x358);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x3c8);
    }
    return p;
}
