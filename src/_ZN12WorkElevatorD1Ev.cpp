//cpp
// @symbol _ZN12WorkElevatorD1Ev
/* recovered: named members + shared header */
#include "WorkElevator.h"
extern "C" {
extern void* func_ov075_0211478c;
extern void* _ZTV17ExclamationSwitch;
void func_0207328c(void*, int, int, void(*)(void*));
void _ZN18MovingMeshColliderD1Ev(void*);
void _ZN5ModelD1Ev(void*);
void _ZN5ActorD2Ev(void*);
void* _ZN12WorkElevatorD1Ev(struct WorkElevator *self) {
    *(void**)((char*)self) = &func_ov075_0211478c;
    func_0207328c(((char*)self)+0x520, 4, 0x1c8, _ZN18MovingMeshColliderD1Ev);
    func_0207328c(((char*)self)+0x320, 4, 0x50, _ZN5ModelD1Ev);
    *(void**)((char*)self) = &_ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev((char*)&self->mMeshCollider);
    _ZN5ModelD1Ev((char*)&self->mModel);
    _ZN5ActorD2Ev(((char*)self));
    return ((char*)self);
}
}
