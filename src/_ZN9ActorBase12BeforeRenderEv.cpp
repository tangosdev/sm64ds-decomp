//cpp
#include "types.h"
// @symbol _ZN9ActorBase12BeforeRenderEv
/* recovered: named members + shared header, real C++ method */
#include "ActorBase.h"
int ActorBase::BeforeRender()
{
  if(mMarkedForDestruction!=0) goto ret0;
  if((unk_013&8)==0) goto ret1;
ret0:
  return 0;
ret1:
  return 1;
}
