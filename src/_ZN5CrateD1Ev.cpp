//cpp
// @symbol _ZN5CrateD1Ev
/* recovered: named members + shared header */
#include "Crate.h"
extern "C" {
void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
void _ZN11ShadowModelD1Ev(void*);
void _ZN12WithMeshClsnD1Ev(void*);
void _ZN18MovingMeshColliderD1Ev(void*);
void _ZN5ModelD1Ev(void*);
void _ZN5ActorD2Ev(void*);
extern int _ZTV5Crate[];
extern int _ZTV17ExclamationSwitch[];
void* _ZN5CrateD1Ev(struct Crate *self) {
    *(int*)((char*)self) = (int)_ZTV5Crate;
    _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos2);
    _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos1);
    _ZN11ShadowModelD1Ev((char*)&self->mShadowModel);
    _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
    *(int*)((char*)self) = (int)_ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev((char*)&self->mMeshCollider);
    _ZN5ModelD1Ev((char*)&self->mModel);
    _ZN5ActorD2Ev(((char*)self));
    return ((char*)self);
}
}
