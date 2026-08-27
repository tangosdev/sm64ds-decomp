//cpp
// @symbol _ZN14dScMgD3DBase_c14BeforeBehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "dScMgD3DBase_c.h"
extern "C" {
extern unsigned int data_020a0db0;
int _ZN8Particle10SysTracker6UpdateEv(void*);
}

int dScMgD3DBase_c::BeforeBehavior()
{
  if(_ZN11dScMgBase_c14BeforeBehaviorEv(((void*)this))==0) return 0;
  if(data_020a0db0 & 1)
    _ZN8Particle10SysTracker6UpdateEv((char*)&mSysTracker);
  return 1;
}
