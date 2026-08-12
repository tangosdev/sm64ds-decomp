extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern int _ZTV12daBakubaku_c[];
int *func_ov032_0211244c(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1080);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV12daBakubaku_c;
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x110);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x150);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x190);
        _ZN9ModelAnimC1Ev((char *)p + 0x34c);
        _ZN11ShadowModelC1Ev((char *)p + 0x3b4);
    }
    return p;
}
