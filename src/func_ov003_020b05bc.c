extern void _ZN9ActorBaseD2Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int VT0[];
extern int VT1[];
extern int VT2[];
extern void *G0;
int *func_ov003_020b05bc(int *t)
{
    t[0] = (int)VT0;
    t[0] = (int)VT1;
    t[0] = (int)VT2;
    _ZN9ActorBaseD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
