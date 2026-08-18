extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV14BlueCoinSwitch[];
int *BlueCoinSwitch_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV14BlueCoinSwitch; }
    return p;
}
