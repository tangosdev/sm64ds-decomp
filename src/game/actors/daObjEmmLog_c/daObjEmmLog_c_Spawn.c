// @symbol daObjEmmLog_c_Spawn
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV13daObjEmmLog_c[];
int *daObjEmmLog_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(808);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV13daObjEmmLog_c; }
    return p;
}
