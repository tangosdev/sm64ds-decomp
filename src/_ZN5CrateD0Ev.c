// @symbol _ZN5CrateD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Crate.h"
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern void* _ZTV5Crate;
extern void* _ZTV8Platform;
extern void* data_020a0eac[];

void* _ZN5CrateD0Ev(struct Crate *self) {
    *(void**)((char*)self) = &_ZTV5Crate;
    _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos2);
    _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos1);
    _ZN11ShadowModelD1Ev((char*)&self->mShadowModel);
    _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
    *(void**)((char*)self) = &_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char*)&self->mMeshCollider);
    _ZN5ModelD1Ev((char*)&self->mModel);
    _ZN5ActorD2Ev(((char*)self));
    _ZN6Memory10DeallocateEPvP4Heap(((char*)self), data_020a0eac[0]);
    return ((char*)self);
}
