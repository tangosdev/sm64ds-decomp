extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void func_ov002_020aed18(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int VT0[];
extern int VT1[];
extern void *G0;
int *_ZN5BullyD0Ev(int *t)
{
    t[0] = (int)VT0;
    t[0] = (int)VT1;
    _ZN11ShadowModelD1Ev((char *)t + 0x370);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x33c);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
