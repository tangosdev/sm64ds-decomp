extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern int VT0[];
int *SpikeBomb_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(432);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x124);
    }
    return p;
}
