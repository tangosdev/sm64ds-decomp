extern int _ZN18MovingMeshColliderD1Ev(void*);
extern int _ZN11ShadowModelD1Ev(void*);
extern int _ZN15TextureSequenceD1Ev(void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN12WithMeshClsnD1Ev(void*);
extern int func_ov002_020aed18(void*);
extern int _ZN6Memory10DeallocateEPvP4Heap(void*,void*);
extern int data_ov079_02127c80[];
extern void* _ZN6Memory11gameHeapPtrE;
int func_ov079_02123798(char *c){
    *(int*)c=(int)data_ov079_02127c80;
    _ZN18MovingMeshColliderD1Ev(c+0x418);
    _ZN11ShadowModelD1Ev(c+0x344);
    _ZN15TextureSequenceD1Ev(c+0x330);
    _ZN9ModelAnimD1Ev(c+0x2cc);
    _ZN12WithMeshClsnD1Ev(c+0x110);
    func_ov002_020aed18(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, _ZN6Memory11gameHeapPtrE);
    return (int)c;
}
