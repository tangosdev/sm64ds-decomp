extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int data_ov022_02114034[];
int *LavaSeesaw_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(804);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)data_ov022_02114034; }
    return p;
}
