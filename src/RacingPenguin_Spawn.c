extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN15TextureSequenceC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN18MovingCylinderClsnC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN7PathPtrC1Ev(void *);
extern int VT0[];
int *RacingPenguin_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(920);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN15TextureSequenceC1Ev((char *)p + 0x138);
        _ZN11ShadowModelC1Ev((char *)p + 0x14c);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x174);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x1a8);
        _ZN7PathPtrC1Ev((char *)p + 0x364);
    }
    return p;
}
