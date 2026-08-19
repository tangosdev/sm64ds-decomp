//cpp
// @symbol _ZN3BooD1Ev
/* recovered: named members + shared header, real derived class (Boo : dCapEnemy_c)
 * -- chains to dCapEnemy_c's out-of-line D1 (func_ov002_020aedbc, not yet renamed;
 * see include/Boo.h for the base-edge evidence) instead of a compiler-
 * synthesized call, since that symbol does not exist under its mangled name
 * in this tree yet. */
#include "Boo.h"
extern "C" {
extern int _ZTV3Boo[];
extern int _ZN11ShadowModelD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN12WithMeshClsnD1Ev(void*);
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int* func_ov002_020aedbc(int*);
int _ZN3BooD1Ev(struct Boo *self) {
  *(int*)((char*)self)=(int)_ZTV3Boo;
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel2);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel1);
  _ZN5ModelD1Ev((char*)&self->mModel);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim);
  _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos);
  func_ov002_020aedbc((int*)((char*)self));
  return (int)((char*)self);
}
}
