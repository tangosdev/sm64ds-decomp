// @symbol RollingRock_Spawn
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV11RollingRock */
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN12dEnemyBase_cC2Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN10dCcAcPos_cC1Ev(void *);
extern void _ZN10dBgCh_ActrC1Ev(void *);
extern int _ZTV11RollingRock[];
int *RollingRock_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(968);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV11RollingRock;
        _ZN5ModelC1Ev((char *)p + 0x110);
        _ZN11ShadowModelC1Ev((char *)p + 0x160);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x1b8);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1f8);
    }
    return p;
}
