extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN15TextureSequenceC1Ev(void *);
extern void _ZN15MaterialChangerC1Ev(void *);
extern void _ZN18TextureTransformerC1Ev(void *);
extern int VT0[];
int *BowserShockwaves_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(536);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN15TextureSequenceC1Ev((char *)p + 0x138);
        _ZN15MaterialChangerC1Ev((char *)p + 0x14c);
        _ZN18TextureTransformerC1Ev((char *)p + 0x160);
        _ZN9ModelAnimC1Ev((char *)p + 0x174);
        _ZN15TextureSequenceC1Ev((char *)p + 0x1d8);
        _ZN15MaterialChangerC1Ev((char *)p + 0x1ec);
        _ZN18TextureTransformerC1Ev((char *)p + 0x200);
    }
    return p;
}
