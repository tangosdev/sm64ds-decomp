extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN14BlendModelAnimC1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern void _ZN11CommonModelC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern int VT0[];
int *KingBobOmb_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1292);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)VT0;
        _ZN12WithMeshClsnC1Ev((char *)p + 0x110);
        _ZN14BlendModelAnimC1Ev((char *)p + 0x2cc);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x33c);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x37c);
        _ZN11CommonModelC1Ev((char *)p + 0x3bc);
        _ZN11ShadowModelC1Ev((char *)p + 0x3f8);
    }
    return p;
}
