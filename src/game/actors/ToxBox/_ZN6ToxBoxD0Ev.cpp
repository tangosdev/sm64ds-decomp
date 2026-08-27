//cpp
// @symbol _ZN6ToxBoxD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "ToxBox.h"
extern "C" {
extern void _ZN10dCcAcPos_cD1Ev(void*);
extern int _ZTV6ToxBox[];
extern int _ZTV10dBgActor_c[];
extern void* data_020a0eac;
void* _ZN6ToxBoxD0Ev(struct ToxBox *self) {
  *(int**)(((char*)self))=_ZTV6ToxBox;
  _ZN10dCcAcPos_cD1Ev((char*)&self->mdCcAcPos_c);
  _ZN10dBgCh_ActrD1Ev((char*)&self->mWithMeshClsn);
  *(int**)(((char*)self))=_ZTV10dBgActor_c;
  _ZN10dBgW_KcMbgD1Ev((char*)&self->mMeshCollider);
  _ZN5ModelD1Ev((char*)&self->mModel);
  _ZN8dActor_cD2Ev(((char*)self));
  _ZN6Memory10DeallocateEPvP4Heap(((char*)self), data_020a0eac);
  return ((char*)self);
}
}
