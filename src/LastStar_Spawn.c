extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern int VT0[];
int *LastStar_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1136);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)VT0;
        _ZN9ModelAnimC1Ev((char *)p + 0x114);
        _ZN5ModelC1Ev((char *)p + 0x178);
        _ZN11ShadowModelC1Ev((char *)p + 0x1c8);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x220);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x260);
    }
    return p;
}
