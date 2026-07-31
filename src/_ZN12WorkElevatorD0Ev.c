// @symbol _ZN12WorkElevatorD0Ev
/* recovered: named members + shared header */
#include "WorkElevator.h"
extern int func_0207328c(void *p, int a, int b, void *f);
extern int _ZN18MovingMeshColliderD1Ev(void *p);
extern int _ZN5ModelD1Ev(void *p);
extern int _ZN5ActorD2Ev(void *p);
extern int _ZN6Memory10DeallocateEPvP4Heap(void *p, void *h);
extern int func_ov075_0211478c[];
extern int _ZTV17ExclamationSwitch[];
extern int *data_020a0eac;
int _ZN12WorkElevatorD0Ev(struct WorkElevator *self) {
    *(int**)(((char *)self)) = func_ov075_0211478c;
    func_0207328c(((char *)self)+0x520, 4, 0x1c8, _ZN18MovingMeshColliderD1Ev);
    func_0207328c(((char *)self)+0x320, 4, 0x50, _ZN5ModelD1Ev);
    *(int**)(((char *)self)) = _ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev((char *)&self->mMeshCollider);
    _ZN5ModelD1Ev((char *)&self->mModel);
    _ZN5ActorD2Ev(((char *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((char *)self), data_020a0eac);
    return (int)((char *)self);
}
