extern void _ZN18TextureTransformerD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern int VT0[];
int *_ZN7TornadoD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN18TextureTransformerD1Ev((char *)t + 0x328);
    _ZN9ModelAnimD1Ev((char *)t + 0x2c4);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x108);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    return t;
}
