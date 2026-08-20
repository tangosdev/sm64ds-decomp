extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN7dCcAc_cD1Ev(void *);
extern void _ZN10dBgCh_ActrD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN12dEnemyBase_cD2Ev(void *);
extern int data_ov064_0211b768[];
int *_ZN7daOts_cD1Ev(int *t)
{
    t[0] = (int)data_ov064_0211b768;
    _ZN11ShadowModelD1Ev((char *)t + 0x370);
    _ZN7dCcAc_cD1Ev((char *)t + 0x33c);
    _ZN10dBgCh_ActrD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    _ZN12dEnemyBase_cD2Ev(t);
    return t;
}
