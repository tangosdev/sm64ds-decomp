extern void func_02017254(void *);
extern void _ZN9ActorBaseD2Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int VT0[];
extern int VT1[];
extern int VT2[];
extern void *G0;
int *func_ov007_020cc070(int *t)
{
    t[0] = (int)VT0;
    func_02017254((char *)t + 0x54);
    t[0] = (int)VT1;
    t[0] = (int)VT2;
    _ZN9ActorBaseD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
