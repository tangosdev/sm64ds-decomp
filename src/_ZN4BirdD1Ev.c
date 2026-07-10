extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern int VT0[];
int *_ZN4BirdD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN11ShadowModelD1Ev((char *)t + 0x138);
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    return t;
}
