//cpp
// @symbol _ZN9ActorBase18MarkForDestructionEv
/* recovered: named members + shared header, real C++ method */
#include "ActorBase.h"


void ActorBase::MarkForDestruction()
{
  if(mMarkedForDestruction!=0) return;
  unsigned char b = (unk_00e==2);
  if(b!=0) return;
  mMarkedForDestruction=1;
  (*(void(**)(unsigned char*))(*(unsigned int*)((unsigned char *)this)+0x30))(((unsigned char *)this));
}
