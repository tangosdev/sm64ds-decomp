extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV7daDgr_c[];
int *func_ov025_02111898(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(820);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV7daDgr_c; }
    return p;
}
