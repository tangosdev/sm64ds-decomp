extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void func_ov002_020aed18(void *);
extern int VT0[];
int *_ZN13MontyMoleRockD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x194);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x160);
    _ZN5ModelD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    return t;
}
