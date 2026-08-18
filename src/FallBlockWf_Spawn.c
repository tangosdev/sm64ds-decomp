extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV16daObjFallBlock_c[];
extern int _ZTV11FallBlockWf[];
int *FallBlockWf_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(844);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjFallBlock_c;
        p[0] = (int)_ZTV11FallBlockWf;
    }
    return p;
}
