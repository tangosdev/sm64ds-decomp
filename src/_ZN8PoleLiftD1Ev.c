extern void _ZN21ExtendingMeshColliderD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern int VT0[];
int *_ZN8PoleLiftD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN21ExtendingMeshColliderD1Ev((char *)t + 0x158);
    _ZN5ModelD1Ev((char *)t + 0xd8);
    _ZN5ActorD1Ev(t);
    return t;
}
