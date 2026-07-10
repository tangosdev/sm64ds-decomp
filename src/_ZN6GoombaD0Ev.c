extern void _ZN15MaterialChangerD1Ev(void *);
extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void func_ov002_020aedbc(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int VT0[];
extern void *G0;
int *_ZN6GoombaD0Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN15MaterialChangerD1Ev((char *)t + 0x3fc);
    _ZN11ShadowModelD1Ev((char *)t + 0x3d4);
    _ZN9ModelAnimD1Ev((char *)t + 0x370);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x1b4);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x180);
    func_ov002_020aedbc(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
