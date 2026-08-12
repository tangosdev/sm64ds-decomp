extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN5EnemyD2Ev(void *);
extern int data_ov032_02113824[];
int *func_ov032_021111a0(int *t)
{
    t[0] = (int)data_ov032_02113824;
    _ZN11ShadowModelD1Ev((char *)t + 0x3b4);
    _ZN9ModelAnimD1Ev((char *)t + 0x34c);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x190);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    return t;
}
