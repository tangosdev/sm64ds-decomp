//cpp
extern "C" {
void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
void _ZN11ShadowModelD1Ev(void*);
void _ZN12WithMeshClsnD1Ev(void*);
void _ZN18MovingMeshColliderD1Ev(void*);
void _ZN5ModelD1Ev(void*);
void _ZN5ActorD1Ev(void*);
extern int _ZTV5Crate[];
extern int _ZTV17ExclamationSwitch[];
void* _ZN5CrateD1Ev(char* self){
    *(int*)self = (int)_ZTV5Crate;
    _ZN25MovingCylinderClsnWithPosD1Ev(self + 0x5a4);
    _ZN25MovingCylinderClsnWithPosD1Ev(self + 0x564);
    _ZN11ShadowModelD1Ev(self + 0x508);
    _ZN12WithMeshClsnD1Ev(self + 0x320);
    *(int*)self = (int)_ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev(self + 0x124);
    _ZN5ModelD1Ev(self + 0xd4);
    _ZN5ActorD1Ev(self);
    return self;
}
}
