// @symbol _ZN8PathLiftD0Ev
/* recovered: named members + shared header */
#include "PathLift.h"
extern int __destroy_arr(void *p, int a, int b, void *f);
extern int _ZN5ModelD1Ev(void *p);
extern int _ZN18MovingMeshColliderD1Ev(void *p);
extern int _ZN5ActorD2Ev(void *p);
extern int _ZN6Memory10DeallocateEPvP4Heap(void *p, void *h);
extern int data_ov002_0210af70[];
extern int _ZTV17ExclamationSwitch[];
extern int *data_020a0eac;
int _ZN8PathLiftD0Ev(struct PathLift *self) {
    *(int**)(((char *)self)) = data_ov002_0210af70;
    __destroy_arr(((char *)self)+0x320, 3, 0x50, _ZN5ModelD1Ev);
    *(int**)(((char *)self)) = _ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev((char *)&self->mMovingMeshCollider);
    _ZN5ModelD1Ev((char *)&self->mModel);
    _ZN5ActorD2Ev(((char *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((char *)self), data_020a0eac);
    return (int)((char *)self);
}
