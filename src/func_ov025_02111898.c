extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int data_ov025_02113760[];
int *func_ov025_02111898(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(820);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)data_ov025_02113760; }
    return p;
}
