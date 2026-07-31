//cpp
// @symbol _ZN17MgBounceAndPounce14BeforeBehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MgBounceAndPounce.h"
extern "C" {
extern unsigned int data_020a0db0;
int _ZN8Particle10SysTracker6UpdateEv(void*);
}

int MgBounceAndPounce::BeforeBehavior()
{
  if(func_ov004_020b0620(((void*)this))==0) return 0;
  if(data_020a0db0 & 1)
    _ZN8Particle10SysTracker6UpdateEv((char*)&unk_47e4);
  return 1;
}
