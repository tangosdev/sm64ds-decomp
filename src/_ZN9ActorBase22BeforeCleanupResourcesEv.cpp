//cpp
// @symbol _ZN9ActorBase22BeforeCleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ActorBase.h"

int ActorBase::BeforeCleanupResources()
{
  int v=unk_048;
  if(v!=0){
    if(func_0204424c(v)==0) goto ret0;
  }
  if(unk_018==0) goto ret1;
ret0:
  return 0;
ret1:
  return 1;
}
