extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern int VT0[];
int *PowerStar_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1220);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)VT0;
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x110);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x150);
        _ZN9ModelAnimC1Ev((char *)p + 0x30c);
        _ZN9ModelAnimC1Ev((char *)p + 0x370);
        _ZN11ShadowModelC1Ev((char *)p + 0x3d4);
    }
    return p;
}
