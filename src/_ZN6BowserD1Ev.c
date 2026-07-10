extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN15TextureSequenceD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern int VT0[];
int *_ZN6BowserD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x360);
    _ZN11ShadowModelD1Ev((char *)t + 0x308);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x14c);
    _ZN15TextureSequenceD1Ev((char *)t + 0x138);
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    return t;
}
