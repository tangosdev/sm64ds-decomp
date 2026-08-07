extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN5EnemyD2Ev(void *);
extern int data_ov064_0211b768[];
int *func_ov064_02115ee0(int *t)
{
    t[0] = (int)data_ov064_0211b768;
    _ZN11ShadowModelD1Ev((char *)t + 0x370);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x33c);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    return t;
}
