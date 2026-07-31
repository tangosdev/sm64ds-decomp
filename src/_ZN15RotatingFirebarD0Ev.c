// @symbol _ZN15RotatingFirebarD0Ev
/* recovered: named members + shared header */
#include "RotatingFirebar.h"
extern int func_0207328c(void *p, int a, int b, void *f);
extern int _ZN5ModelD1Ev(void *p);
extern int _ZN18MovingMeshColliderD1Ev(void *p);
extern int _ZN5ActorD2Ev(void *p);
extern int _ZN6Memory10DeallocateEPvP4Heap(void *p, void *h);
extern int _ZTV15RotatingFirebar[];
extern int _ZN19CylinderClsnWithPosD1Ev[];
extern int _ZTV17ExclamationSwitch[];
extern int *data_020a0eac;
int _ZN15RotatingFirebarD0Ev(struct RotatingFirebar *self) {
    *(int**)(((char *)self)) = _ZTV15RotatingFirebar;
    func_0207328c(((char *)self)+0x360, 8, 0x3c, _ZN19CylinderClsnWithPosD1Ev);
    *(int**)(((char *)self)) = _ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev((char *)&self->mMeshCollider);
    _ZN5ModelD1Ev((char *)&self->mModel);
    _ZN5ActorD2Ev(((char *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((char *)self), data_020a0eac);
    return (int)((char *)self);
}
