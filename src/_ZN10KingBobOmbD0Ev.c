extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN11CommonModelD1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN14BlendModelAnimD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void func_ov002_020aed18(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int VT0[];
extern void *G0;
int *_ZN10KingBobOmbD0Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN11ShadowModelD1Ev((char *)t + 0x3f8);
    _ZN11CommonModelD1Ev((char *)t + 0x3bc);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x37c);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x33c);
    _ZN14BlendModelAnimD1Ev((char *)t + 0x2cc);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
