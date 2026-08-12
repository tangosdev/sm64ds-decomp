extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int _ZTV5Stump[];
int *Stump_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(816);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)_ZTV5Stump; }
    return p;
}
