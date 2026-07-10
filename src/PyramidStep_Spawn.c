extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern int VT0[];
int *PyramidStep_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(932);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)VT0;
        _ZN5ModelC1Ev((char *)p + 0x320);
    }
    return p;
}
