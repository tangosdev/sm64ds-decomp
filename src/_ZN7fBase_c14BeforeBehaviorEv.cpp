//cpp
#include "types.h"
// @symbol _ZN9ActorBase14BeforeBehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "ActorBase.h"
int ActorBase::BeforeBehavior()
{
  if(shouldBeKilled!=0) goto ret0;
  if((unk_013&2)==0) goto ret1;
ret0:
  return 0;
ret1:
  return 1;
}
