extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN15TextureSequenceC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern int VT0[];
int *Bowser_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1108);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN15TextureSequenceC1Ev((char *)p + 0x138);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x14c);
        _ZN11ShadowModelC1Ev((char *)p + 0x308);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x360);
    }
    return p;
}
