extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV12SwitchPillar[];
int *SwitchPillar_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV12SwitchPillar; }
    return p;
}
