extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int _ZTV10LavaSeesaw[];
int *LavaSeesaw_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(804);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)_ZTV10LavaSeesaw; }
    return p;
}
