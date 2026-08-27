extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN8dActor_cC2Ev(void *);
extern int _ZTV11SoundObject[];
int *SoundObject_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(228);
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)_ZTV11SoundObject; }
    return p;
}
