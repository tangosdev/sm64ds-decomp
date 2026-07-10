extern void _ZN18MovingMeshColliderD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern int VT0[];
extern int VT1[];
extern int VT2[];
int *_ZN12FallBlockBbhD1Ev(int *t)
{
    t[0] = (int)VT0;
    t[0] = (int)VT1;
    t[0] = (int)VT2;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    return t;
}
