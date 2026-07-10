extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void _ZN18MovingMeshColliderD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern int VT0[];
extern int VT1[];
int *_ZN8SignPostD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x3c8);
    _ZN11ShadowModelD1Ev((char *)t + 0x358);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x320);
    t[0] = (int)VT1;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    return t;
}
