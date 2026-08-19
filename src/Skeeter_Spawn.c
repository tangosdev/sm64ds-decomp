extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN12dEnemyBase_cC2Ev(void *);
extern void _ZN10dCcAcPos_cC1Ev(void *);
extern void _ZN10dBgCh_ActrC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern int _ZTV7Skeeter[];
int *Skeeter_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(944);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV7Skeeter;
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x150);
        _ZN9ModelAnimC1Ev((char *)p + 0x30c);
    }
    return p;
}
