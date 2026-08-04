// @symbol _ZN18RickshawPlatformBsD0Ev
/* recovered: named members + shared header */
#include "RickshawPlatformBs.h"
extern int __destroy_arr(void *p, int a, int b, void *f);
extern int _ZN18MovingMeshColliderD1Ev(void *p);
extern int _ZN5ModelD1Ev(void *p);
extern int _ZN5ActorD2Ev(void *p);
extern int _ZN6Memory10DeallocateEPvP4Heap(void *p, void *h);
extern int _ZTV18RickshawPlatformBs[];
extern int data_ov002_02108d94[];
extern int _ZTV8Platform[];
extern int *data_020a0eac;
int _ZN18RickshawPlatformBsD0Ev(struct RickshawPlatformBs *self) {
    *(int**)(((char *)self)) = _ZTV18RickshawPlatformBs;
    *(int**)(((char *)self)) = data_ov002_02108d94;
    __destroy_arr(((char *)self)+0x4b0, 5, 0x1c8, _ZN18MovingMeshColliderD1Ev);
    __destroy_arr(((char *)self)+0x320, 5, 0x50, _ZN5ModelD1Ev);
    *(int**)(((char *)self)) = _ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)&self->mMovingMeshCollider);
    _ZN5ModelD1Ev((char *)&self->mModel);
    _ZN5ActorD2Ev(((char *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((char *)self), data_020a0eac);
    return (int)((char *)self);
}
