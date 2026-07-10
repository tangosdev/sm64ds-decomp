extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern void _ZN18TextureTransformerC1Ev(void *);
extern int VT0[];
int *HugeWater_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(820);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)VT0;
        _ZN18TextureTransformerC1Ev((char *)p + 0x320);
    }
    return p;
}
