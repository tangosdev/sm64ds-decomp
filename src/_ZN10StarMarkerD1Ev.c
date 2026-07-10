extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern int VT0[];
int *_ZN10StarMarkerD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN11ShadowModelD1Ev((char *)t + 0x164);
    _ZN5ModelD1Ev((char *)t + 0x114);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    return t;
}
