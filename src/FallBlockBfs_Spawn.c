extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int VT0[];
extern int VT1[];
int *FallBlockBfs_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(844);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)VT0;
        p[0] = (int)VT1;
    }
    return p;
}
