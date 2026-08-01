extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void func_ov002_020aed18(void *);
extern int data_ov032_02113824[];
int *func_ov032_021111a0(int *t)
{
    t[0] = (int)data_ov032_02113824;
    _ZN11ShadowModelD1Ev((char *)t + 0x3b4);
    _ZN9ModelAnimD1Ev((char *)t + 0x34c);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x190);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    return t;
}
