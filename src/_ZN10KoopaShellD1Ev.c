extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void func_ov002_020aed18(void *);
extern int VT0[];
int *_ZN10KoopaShellD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x378);
    _ZN11ShadowModelD1Ev((char *)t + 0x350);
    _ZN5ModelD1Ev((char *)t + 0x300);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x144);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    return t;
}
