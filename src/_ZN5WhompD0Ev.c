// @symbol _ZN5WhompD0Ev
/* recovered: named members + shared header */
#include "Whomp.h"
extern int _ZN18MovingMeshColliderD1Ev(void*);
extern int _ZN11ShadowModelD1Ev(void*);
extern int _ZN15TextureSequenceD1Ev(void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN12WithMeshClsnD1Ev(void*);
extern int _ZN5EnemyD2Ev(void*);
extern int _ZN6Memory10DeallocateEPvP4Heap(void*,void*);
extern int _ZTV5Whomp[];
extern void* data_020a0eac;
int _ZN5WhompD0Ev(struct Whomp *self) {
    *(int*)((char *)self)=(int)_ZTV5Whomp;
    _ZN18MovingMeshColliderD1Ev((char *)&self->mMovingMeshCollider);
    _ZN11ShadowModelD1Ev((char *)&self->mShadowModel);
    _ZN15TextureSequenceD1Ev((char *)&self->mTextureSequence);
    _ZN9ModelAnimD1Ev((char *)&self->mModelAnim);
    _ZN12WithMeshClsnD1Ev((char *)&self->mWithMeshClsn);
    _ZN5EnemyD2Ev(((char *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((char *)self), data_020a0eac);
    return (int)((char *)self);
}
