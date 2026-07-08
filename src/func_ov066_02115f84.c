extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void* func_0207328c(void*, int, int, void*);
extern void _ZN15TextureSequenceD1Ev(void*);
extern void _ZN11ShadowModelD1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN14BlendModelAnimD1Ev(void*);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern void _ZN5ActorD1Ev(void*);
extern void* _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
extern int data_ov066_0211ad64[];
extern int func_020072c0[];
extern int _ZTV8Platform[];
extern void* _ZN6Memory11gameHeapPtrE;
void* func_ov066_02115f84(char* c) {
    *(int**)c = data_ov066_0211ad64;
    _ZN18MovingMeshColliderD1Ev(c+0x674);
    func_0207328c(c+0x4dc, 0x14, 0xc, func_020072c0);
    _ZN15TextureSequenceD1Ev(c+0x448);
    _ZN11ShadowModelD1Ev(c+0x420);
    _ZN5ModelD1Ev(c+0x3d0);
    _ZN14BlendModelAnimD1Ev(c+0x360);
    _ZN25MovingCylinderClsnWithPosD1Ev(c+0x320);
    *(int**)c = _ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev(c+0x124);
    _ZN5ModelD1Ev(c+0xd4);
    _ZN5ActorD1Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, _ZN6Memory11gameHeapPtrE);
    return c;
}
