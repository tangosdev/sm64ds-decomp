extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN11CommonModelC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN18MovingCylinderClsnC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern int VT0[];
int *Coin_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(948);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        _ZN11CommonModelC1Ev((char *)p + 0xd8);
        _ZN11CommonModelC1Ev((char *)p + 0x114);
        _ZN11ShadowModelC1Ev((char *)p + 0x150);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x178);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x1ac);
    }
    return p;
}
