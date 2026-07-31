// @symbol _ZN3BooD1Ev
/* recovered: named members + shared header */
#include "Boo.h"
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
