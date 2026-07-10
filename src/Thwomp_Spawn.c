extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern void _ZN15TextureSequenceC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern int VT0[];
extern int VT1[];
int *Thwomp_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(932);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)VT0;
        _ZN15TextureSequenceC1Ev((char *)p + 0x324);
        _ZN11ShadowModelC1Ev((char *)p + 0x338);
        p[0] = (int)VT1;
    }
    return p;
}
