extern void _ZN18TextureTransformerD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void func_ov002_020aed18(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int VT0[];
extern void *G0;
int *_ZN9SubmarineD0Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN18TextureTransformerD1Ev((char *)t + 0x178);
    _ZN9ModelAnimD1Ev((char *)t + 0x114);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
