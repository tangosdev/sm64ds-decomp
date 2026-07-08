extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern void _ZN11ShadowModelD1Ev(void*);
extern void _ZN12WithMeshClsnD1Ev(void*);
extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ActorD1Ev(void*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
extern void* data_ov098_0213c534;
extern void* _ZTV8Platform;
extern void* _ZN6Memory11gameHeapPtrE[];

void* func_ov098_021380bc(char* c)
{
    *(void**)c = &data_ov098_0213c534;
    _ZN25MovingCylinderClsnWithPosD1Ev(c+0x5a4);
    _ZN25MovingCylinderClsnWithPosD1Ev(c+0x564);
    _ZN11ShadowModelD1Ev(c+0x508);
    _ZN12WithMeshClsnD1Ev(c+0x320);
    *(void**)c = &_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev(c+0x124);
    _ZN5ModelD1Ev(c+0xd4);
    _ZN5ActorD1Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, _ZN6Memory11gameHeapPtrE[0]);
    return c;
}
