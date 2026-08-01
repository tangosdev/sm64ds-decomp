extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int data_ov032_021138e0[];
int *HugeCover_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(800);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)data_ov032_021138e0; }
    return p;
}
