//cpp
// @symbol _ZN17MgBounceAndPounce18AfterInitResourcesEj
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "MgBounceAndPounce.h"
extern "C" {
int _ZN8Particle10SysTracker10InitialiseEv(void*);
int _ZN17MgBounceAndPounce18AfterInitResourcesEj(struct MgBounceAndPounce *self) {
  func_ov004_020b08f0(((void*)self));
  return _ZN8Particle10SysTracker10InitialiseEv((char*)&self->unk_47e4);
}
}
