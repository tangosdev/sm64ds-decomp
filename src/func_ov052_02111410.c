extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int data_ov052_02112520[];
int *func_ov052_02111410(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(808);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)data_ov052_02112520; }
    return p;
}
