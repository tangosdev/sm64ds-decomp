extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN14BlendModelAnimD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void func_ov002_020aed18(void *);
extern int VT0[];
int *_ZN6KleptoD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN11ShadowModelD1Ev((char *)t + 0x3a4);
    _ZN14BlendModelAnimD1Ev((char *)t + 0x334);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x178);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x144);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    return t;
}
