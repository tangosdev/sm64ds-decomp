extern int func_0207328c(void *p, int a, int b, void *f);
extern int _ZN18MovingMeshColliderD1Ev(void *p);
extern int _ZN5ModelD1Ev(void *p);
extern int _ZN5ActorD1Ev(void *p);
extern int _ZN6Memory10DeallocateEPvP4Heap(void *p, void *h);
extern int data_ov036_02113ecc[];
extern int data_ov002_02108d94[];
extern int _ZTV8Platform[];
extern int *_ZN6Memory11gameHeapPtrE;
int func_ov036_0211200c(char *c){
    *(int**)(c) = data_ov036_02113ecc;
    *(int**)(c) = data_ov002_02108d94;
    func_0207328c(c+0x4b0, 5, 0x1c8, _ZN18MovingMeshColliderD1Ev);
    func_0207328c(c+0x320, 5, 0x50, _ZN5ModelD1Ev);
    *(int**)(c) = _ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev(c+0x124);
    _ZN5ModelD1Ev(c+0xd4);
    _ZN5ActorD1Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, _ZN6Memory11gameHeapPtrE);
    return (int)c;
}
