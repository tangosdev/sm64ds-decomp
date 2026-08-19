extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN12dEnemyBase_cC2Ev(void *);
extern void _ZN10dBgCh_ActrC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN15TextureSequenceC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN10dBgW_KcMbgC1Ev(void *);
extern int _ZTV5Whomp[];
int *Whomp_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x610);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV5Whomp;
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x110);
        _ZN9ModelAnimC1Ev((char *)p + 0x2cc);
        _ZN15TextureSequenceC1Ev((char *)p + 0x330);
        _ZN11ShadowModelC1Ev((char *)p + 0x344);
        _ZN10dBgW_KcMbgC1Ev((char *)p + 0x418);
    }
    return p;
}
