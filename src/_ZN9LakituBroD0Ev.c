extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN15TextureSequenceD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void func_ov002_020aed18(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int VT0[];
extern void *G0;
int *_ZN9LakituBroD0Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN11ShadowModelD1Ev((char *)t + 0x218);
    _ZN11ShadowModelD1Ev((char *)t + 0x1f0);
    _ZN15TextureSequenceD1Ev((char *)t + 0x1d8);
    _ZN9ModelAnimD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
