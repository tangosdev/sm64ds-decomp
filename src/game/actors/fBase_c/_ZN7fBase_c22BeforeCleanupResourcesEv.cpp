//cpp
// @symbol _ZN7fBase_c22BeforeCleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "fBase_c.h"

int fBase_c::BeforeCleanupResources()
{
  int v=(int)unk_048;
  if(v!=0){
    if(func_0204424c(v)==0) goto ret0;
  }
  if(sceneNode.unk_004==0) goto ret1;
ret0:
  return 0;
ret1:
  return 1;
}
