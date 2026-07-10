extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN21ExtendingMeshColliderC1Ev(void *);
extern int VT0[];
int *ExtendingPlatform_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(808);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        _ZN5ModelC1Ev((char *)p + 0xd8);
        _ZN21ExtendingMeshColliderC1Ev((char *)p + 0x158);
    }
    return p;
}
