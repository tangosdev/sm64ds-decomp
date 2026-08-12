extern void _ZN5ModelD1Ev(void *);
extern void _ZN5ActorD2Ev(void *);
extern int _ZTV21ClockPaintingPendulum[];
int *_ZN21ClockPaintingPendulumD1Ev(int *t)
{
    t[0] = (int)_ZTV21ClockPaintingPendulum;
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
