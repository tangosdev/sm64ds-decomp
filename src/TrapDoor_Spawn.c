extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN18MovingMeshColliderC1Ev(void *);
extern int VT0[];
int *TrapDoor_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(852);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN18MovingMeshColliderC1Ev((char *)p + 0x15c);
    }
    return p;
}
