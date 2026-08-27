extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN12dEnemyBase_cC2Ev(void *);
extern void _ZN10dCcAcPos_cC1Ev(void *);
extern void _ZN10dBgCh_ActrC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern int _ZTV12daBakubaku_c[];
int *func_ov032_0211244c(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1080);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV12daBakubaku_c;
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x110);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x150);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x190);
        _ZN9ModelAnimC1Ev((char *)p + 0x34c);
        _ZN11ShadowModelC1Ev((char *)p + 0x3b4);
    }
    return p;
}
