extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int VT[];
int *SkiLift_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(852);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)VT; }
    return p;
}
