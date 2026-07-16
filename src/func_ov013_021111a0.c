extern void _ZN5ModelD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern int data_ov013_02112128[];
int *func_ov013_021111a0(int *t)
{
    t[0] = (int)data_ov013_02112128;
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    return t;
}
