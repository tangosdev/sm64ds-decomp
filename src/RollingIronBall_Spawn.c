extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN18MovingCylinderClsnC1Ev(void *);
extern void _ZN7PathPtrC1Ev(void *);
extern int VT0[];
int *RollingIronBall_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1020);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)VT0;
        _ZN12WithMeshClsnC1Ev((char *)p + 0x110);
        _ZN5ModelC1Ev((char *)p + 0x2cc);
        _ZN11ShadowModelC1Ev((char *)p + 0x31c);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x374);
        _ZN7PathPtrC1Ev((char *)p + 0x3f4);
    }
    return p;
}
