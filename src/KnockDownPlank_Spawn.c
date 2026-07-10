extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern int VT0[];
int *KnockDownPlank_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(924);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)VT0;
        _ZN11ShadowModelC1Ev((char *)p + 0x320);
    }
    return p;
}
