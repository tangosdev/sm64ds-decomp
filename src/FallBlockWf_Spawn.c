extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int data_ov098_0213c5bc[];
extern int _ZTV11FallBlockWf[];
int *FallBlockWf_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(844);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)data_ov098_0213c5bc;
        p[0] = (int)_ZTV11FallBlockWf;
    }
    return p;
}
