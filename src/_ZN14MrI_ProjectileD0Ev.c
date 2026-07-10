extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int VT0[];
extern void *G0;
int *_ZN14MrI_ProjectileD0Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x13c);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0xfc);
    _ZN11ShadowModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
