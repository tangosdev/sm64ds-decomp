//cpp
extern "C" {
void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
void _ZN11ShadowModelD1Ev(void*);
void _ZN12WithMeshClsnD1Ev(void*);
void _ZN18MovingMeshColliderD1Ev(void*);
void _ZN5ModelD1Ev(void*);
void _ZN5ActorD1Ev(void*);
extern int data_ov098_0213c534[];
extern int _ZTV8Platform[];
void* func_ov098_02138040(char* self){
    *(int*)self = (int)data_ov098_0213c534;
    _ZN25MovingCylinderClsnWithPosD1Ev(self + 0x5a4);
    _ZN25MovingCylinderClsnWithPosD1Ev(self + 0x564);
    _ZN11ShadowModelD1Ev(self + 0x508);
    _ZN12WithMeshClsnD1Ev(self + 0x320);
    *(int*)self = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev(self + 0x124);
    _ZN5ModelD1Ev(self + 0xd4);
    _ZN5ActorD1Ev(self);
    return self;
}
}
