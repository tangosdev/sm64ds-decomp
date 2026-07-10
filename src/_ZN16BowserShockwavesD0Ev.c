extern void _ZN18TextureTransformerD1Ev(void *);
extern void _ZN15MaterialChangerD1Ev(void *);
extern void _ZN15TextureSequenceD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int VT0[];
extern void *G0;
int *_ZN16BowserShockwavesD0Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN18TextureTransformerD1Ev((char *)t + 0x200);
    _ZN15MaterialChangerD1Ev((char *)t + 0x1ec);
    _ZN15TextureSequenceD1Ev((char *)t + 0x1d8);
    _ZN9ModelAnimD1Ev((char *)t + 0x174);
    _ZN18TextureTransformerD1Ev((char *)t + 0x160);
    _ZN15MaterialChangerD1Ev((char *)t + 0x14c);
    _ZN15TextureSequenceD1Ev((char *)t + 0x138);
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
