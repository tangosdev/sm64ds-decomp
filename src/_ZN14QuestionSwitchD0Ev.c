// @symbol _ZN14QuestionSwitchD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "QuestionSwitch.h"
extern int _ZTV8Platform[];
extern void *data_020a0eac;
int *_ZN14QuestionSwitchD0Ev(struct QuestionSwitch *self) {
    ((int *)self)[0] = (int)_ZTV14QuestionSwitch;
    _ZN9ModelAnimD1Ev((char *)&self->mModelAnim);
    _ZN18MovingMeshColliderD1Ev((char *)&self->mMovingMeshCollider);
    _ZN18MovingMeshColliderD1Ev((char *)&self->unk_324);
    ((int *)self)[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)&self->unk_124);
    _ZN5ModelD1Ev((char *)&self->mModel);
    _ZN5ActorD2Ev(((int *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((int *)self), data_020a0eac);
    return ((int *)self);
}
