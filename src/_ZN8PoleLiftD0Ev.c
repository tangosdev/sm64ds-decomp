extern void _ZN21ExtendingMeshColliderD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int VT0[];
extern void *G0;
int *_ZN8PoleLiftD0Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN21ExtendingMeshColliderD1Ev((char *)t + 0x158);
    _ZN5ModelD1Ev((char *)t + 0xd8);
    _ZN5ActorD1Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
