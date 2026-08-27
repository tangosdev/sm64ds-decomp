// @symbol _ZN8MadPianoD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
/* recovered: named members + shared header */
#include "MadPiano.h"
extern int __destroy_arr(void*, int, int, void*);
extern void _ZN10dCcAcPos_cD1Ev(void*);
extern void* _ZTV8MadPiano;
extern void* _ZTV10dBgActor_c;
void* _ZN8MadPianoD1Ev(struct MadPiano *self) {
  *(void**)((void*)self) = &_ZTV8MadPiano;
  _ZN10dBgCh_ActrD1Ev((char*)&self->mWithMeshClsn);
  __destroy_arr((char*)((void*)self)+0x48c, 2, 0x40, &_ZN10dCcAcPos_cD1Ev);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel3);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel2);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel1);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim);
  *(void**)((void*)self) = &_ZTV10dBgActor_c;
  _ZN10dBgW_KcMbgD1Ev((char*)&self->mMeshCollider);
  _ZN5ModelD1Ev((char*)&self->mModel);
  _ZN8dActor_cD2Ev(((void*)self));
  return ((void*)self);
}
