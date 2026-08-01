extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int data_ov025_02113760[];
int *func_ov025_02111898(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(820);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)data_ov025_02113760; }
    return p;
}
