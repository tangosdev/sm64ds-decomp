// @symbol _ZN5WhompD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Whomp.h"
extern void _ZN15TextureSequenceD1Ev(void*);
extern int func_ov002_020aed18(int* x);
void* _ZN5WhompD1Ev(struct Whomp *self) {
  *(int*)((char*)self) = (int)_ZTV5Whomp;
  _ZN18MovingMeshColliderD1Ev((char*)&self->mMovingMeshCollider);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel);
  _ZN15TextureSequenceD1Ev((char*)&self->mTextureSequence);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim);
  _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
  func_ov002_020aed18((int*)((char*)self));
  return ((char*)self);
}
