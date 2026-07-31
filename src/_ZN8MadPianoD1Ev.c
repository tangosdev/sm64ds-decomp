// @symbol _ZN8MadPianoD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
/* recovered: named members + shared header */
#include "MadPiano.h"
extern int func_0207328c(void*, int, int, void*);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern void* _ZTV8MadPiano;
extern void* _ZTV17ExclamationSwitch;
void* _ZN8MadPianoD1Ev(struct MadPiano *self) {
  *(void**)((void*)self) = &_ZTV8MadPiano;
  _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
  func_0207328c((char*)((void*)self)+0x48c, 2, 0x40, &_ZN25MovingCylinderClsnWithPosD1Ev);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel3);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel2);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel1);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim);
  *(void**)((void*)self) = &_ZTV17ExclamationSwitch;
  _ZN18MovingMeshColliderD1Ev((char*)&self->mMeshCollider);
  _ZN5ModelD1Ev((char*)&self->mModel);
  _ZN5ActorD2Ev(((void*)self));
  return ((void*)self);
}
