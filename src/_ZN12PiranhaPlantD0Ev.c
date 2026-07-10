extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void func_ov002_020aed18(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int VT0[];
extern void *G0;
int *_ZN12PiranhaPlantD0Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x3e8);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x3b4);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x380);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x1c4);
    _ZN5ModelD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
