extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN18MovingCylinderClsnC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN7PathPtrC1Ev(void *);
extern int VT0[];
int *UkikiStar_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(972);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x138);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x160);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x194);
        _ZN7PathPtrC1Ev((char *)p + 0x398);
    }
    return p;
}
