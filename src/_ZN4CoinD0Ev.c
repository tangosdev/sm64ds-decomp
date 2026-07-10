extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN11CommonModelD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int VT0[];
extern void *G0;
int *_ZN4CoinD0Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x1ac);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x178);
    _ZN11ShadowModelD1Ev((char *)t + 0x150);
    _ZN11CommonModelD1Ev((char *)t + 0x114);
    _ZN11CommonModelD1Ev((char *)t + 0xd8);
    _ZN5ActorD1Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
