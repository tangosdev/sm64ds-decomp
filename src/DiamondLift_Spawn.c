extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV11DiamondLift[];
int *DiamondLift_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV11DiamondLift; }
    return p;
}
