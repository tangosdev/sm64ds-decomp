extern void _ZN5ModelD1Ev(void *);
extern void _ZN5ActorD2Ev(void *);
extern int VT0[];
int *func_ov015_021111a0(int *t)
{
    t[0] = (int)VT0;
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
