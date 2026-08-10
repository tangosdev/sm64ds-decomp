// @symbol _ZN11PyramidLiftD0Ev
/* recovered: named members + shared header */
#include "PyramidLift.h"
extern int __destroy_arr(void *p, int a, int b, void *f);
extern int _ZN5ModelD1Ev(void *p);
extern int _ZN18MovingMeshColliderD1Ev(void *p);
extern int _ZN5ActorD2Ev(void *p);
extern int _ZN6Memory10DeallocateEPvP4Heap(void *p, void *h);
extern int _ZTV11PyramidLift[];
extern int _ZN7Vector3D1Ev[];
extern int _ZTV8Platform[];
extern int *data_020a0eac;
int _ZN11PyramidLiftD0Ev(struct PyramidLift *self) {
    *(int**)(((char *)self)) = _ZTV11PyramidLift;
    __destroy_arr(((char *)self)+0x37c, 0xa, 0xc, _ZN7Vector3D1Ev);
    _ZN5ModelD1Ev((char *)&self->mModel2);
    *(int**)(((char *)self)) = _ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)&self->mMeshCollider);
    _ZN5ModelD1Ev((char *)&self->mModel1);
    _ZN5ActorD2Ev(((char *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((char *)self), data_020a0eac);
    return (int)((char *)self);
}
