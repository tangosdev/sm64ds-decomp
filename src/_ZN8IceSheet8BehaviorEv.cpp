//cpp
// @symbol _ZN8IceSheet8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "IceSheet.h"
#include "dBgW.h"
extern "C" {
}

int IceSheet::Behavior()
{
  if (!((dBgW *)((char*)&(*(u8 *)&mMeshCollider)))->IsEnabled())
    ((dBgW *)(&mMeshCollider))->Enable((dActor_c *)(((char*)this)));
  return 1;
}
