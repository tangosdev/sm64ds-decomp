extern void _ZN11CommonModelD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern int VT0[];
int *_ZN18RotatingPlatformRrD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN11CommonModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    return t;
}
