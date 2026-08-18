//cpp
// @symbol _ZN12WorkElevatorD1Ev
/* recovered: named members + shared header */
#include "WorkElevator.h"
extern "C" {
extern void* _ZTV12WorkElevator;
extern void* _ZTV10dBgActor_c;
void __destroy_arr(void*, int, int, void(*)(void*));
void _ZN18MovingMeshColliderD1Ev(void*);
void _ZN5ModelD1Ev(void*);
void _ZN8dActor_cD2Ev(void*);
void* _ZN12WorkElevatorD1Ev(struct WorkElevator *self) {
    *(void**)((char*)self) = &_ZTV12WorkElevator;
    __destroy_arr(((char*)self)+0x520, 4, 0x1c8, _ZN18MovingMeshColliderD1Ev);
    __destroy_arr(((char*)self)+0x320, 4, 0x50, _ZN5ModelD1Ev);
    *(void**)((char*)self) = &_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char*)&self->mMeshCollider);
    _ZN5ModelD1Ev((char*)&self->mModel);
    _ZN8dActor_cD2Ev(((char*)self));
    return ((char*)self);
}
}
