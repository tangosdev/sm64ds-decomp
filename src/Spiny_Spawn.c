extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN18MovingCylinderClsnC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern int VT0[];
int *Spiny_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1004);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN9ModelAnimC1Ev((char *)p + 0x124);
        _ZN11ShadowModelC1Ev((char *)p + 0x188);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x1b0);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x1e4);
    }
    return p;
}
