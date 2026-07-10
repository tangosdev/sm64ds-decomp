extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern int VT0[];
int *MrI_Projectile_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(820);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        _ZN11ShadowModelC1Ev((char *)p + 0xd4);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0xfc);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x13c);
    }
    return p;
}
