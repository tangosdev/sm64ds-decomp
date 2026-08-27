//cpp
// @symbol _ZN6ToxBoxD1Ev
/* recovered: named members + shared header */
#include "ToxBox.h"
extern "C" {
extern int _ZN10dCcAcPos_cD1Ev(void*);
extern int _ZN10dBgCh_ActrD1Ev(void*);
extern int _ZN10dBgW_KcMbgD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN8dActor_cD2Ev(void*);
extern void* _ZTV6ToxBox;
extern void* _ZTV10dBgActor_c;
void* _ZN6ToxBoxD1Ev(struct ToxBox *self) {
  *(void**)((char*)self) = &_ZTV6ToxBox;
  _ZN10dCcAcPos_cD1Ev((char*)&self->mdCcAcPos_c);
  _ZN10dBgCh_ActrD1Ev((char*)&self->mWithMeshClsn);
  *(void**)((char*)self) = &_ZTV10dBgActor_c;
  _ZN10dBgW_KcMbgD1Ev((char*)&self->mMeshCollider);
  _ZN5ModelD1Ev((char*)&self->mModel);
  _ZN8dActor_cD2Ev(((char*)self));
  return ((char*)self);
}
}
