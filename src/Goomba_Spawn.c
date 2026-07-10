extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8CapEnemyC2Ev(void *);
extern void _ZN18MovingCylinderClsnC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN15MaterialChangerC1Ev(void *);
extern int VT0[];
int *Goomba_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1144);
    if (p) {
        _ZN8CapEnemyC2Ev(p);
        p[0] = (int)VT0;
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x180);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x1b4);
        _ZN9ModelAnimC1Ev((char *)p + 0x370);
        _ZN11ShadowModelC1Ev((char *)p + 0x3d4);
        _ZN15MaterialChangerC1Ev((char *)p + 0x3fc);
    }
    return p;
}
