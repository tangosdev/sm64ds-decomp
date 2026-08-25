//cpp
// @symbol _ZN7daTrs_cD1Ev
/* recovered: named members + shared header, real derived class (daTrs_c : dCapEnemy_c)
 * -- chains to dCapEnemy_c's out-of-line D1 (func_ov002_020aedbc, not yet renamed;
 * see include/daTrs_c.h for the base-edge evidence) instead of a compiler-
 * synthesized call, since that symbol does not exist under its mangled name
 * in this tree yet. */
#include "daTrs_c.h"
extern "C" {
extern int _ZTV7daTrs_c[];
extern int _ZN11ShadowModelD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN10dBgCh_ActrD1Ev(void*);
extern int _ZN10dCcAcPos_cD1Ev(void*);
extern int* func_ov002_020aedbc(int*);
int _ZN7daTrs_cD1Ev(struct daTrs_c *self) {
  *(int*)((char*)self)=(int)_ZTV7daTrs_c;
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel2);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel1);
  _ZN5ModelD1Ev((char*)&self->mBodyModel);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim);
  _ZN10dBgCh_ActrD1Ev((char*)&self->mWithMeshClsn);
  _ZN10dCcAcPos_cD1Ev((char*)&self->mdCcAcPos_c);
  func_ov002_020aedbc((int*)((char*)self));
  return (int)((char*)self);
}
}
