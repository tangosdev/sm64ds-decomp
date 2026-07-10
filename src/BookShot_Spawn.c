extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern int VT0[];
int *BookShot_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1108);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)VT0;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN5ModelC1Ev((char *)p + 0x174);
        _ZN11ShadowModelC1Ev((char *)p + 0x1c4);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x21c);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x25c);
    }
    return p;
}
