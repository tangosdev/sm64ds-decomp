extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern int VT0[];
int *_ZN14FlameChompFireD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x130);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xfc);
    _ZN11ShadowModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    return t;
}
