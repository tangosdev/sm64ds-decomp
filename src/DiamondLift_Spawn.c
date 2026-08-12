extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int _ZTV11DiamondLift[];
int *DiamondLift_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(800);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)_ZTV11DiamondLift; }
    return p;
}
