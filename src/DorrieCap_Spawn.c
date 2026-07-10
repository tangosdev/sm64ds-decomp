extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void func_ov001_020ab3c4(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN18MovingCylinderClsnC1Ev(void *);
extern int VT0[];
int *DorrieCap_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(388);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        func_ov001_020ab3c4((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0xf0);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x140);
    }
    return p;
}
