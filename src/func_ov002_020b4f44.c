extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN18MovingMeshColliderD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int data_ov002_02108e5c[];
extern int _ZTV8Platform[];
extern void *_ZN6Memory11gameHeapPtrE;
int *func_ov002_020b4f44(int *t)
{
    t[0] = (int)data_ov002_02108e5c;
    _ZN9ModelAnimD1Ev((char *)t + 0x6b4);
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x4ec);
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x324);
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, _ZN6Memory11gameHeapPtrE);
    return t;
}
