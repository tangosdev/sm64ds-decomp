extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern int VT0[];
int *_ZN4TrapD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    return t;
}
