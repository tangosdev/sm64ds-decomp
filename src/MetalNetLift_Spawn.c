extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern void _ZN7PathPtrC1Ev(void *);
extern int VT0[];
int *MetalNetLift_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(872);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)VT0;
        _ZN7PathPtrC1Ev((char *)p + 0x360);
    }
    return p;
}
