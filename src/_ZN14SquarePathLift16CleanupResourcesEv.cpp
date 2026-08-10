//cpp
// @symbol _ZN14SquarePathLift16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "SquarePathLift.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern "C" {
struct SFP{int x;};
extern SFP data_ov052_021125a0[2];
}

int SquarePathLift::CleanupResources()
{
  if(((MeshColliderBase *)((char*)&(*(u8 *)&mMeshCollider)))->IsEnabled())
    ((MeshColliderBase *)((char*)&(*(u8 *)&mMeshCollider)))->Disable();
  ((SharedFilePtr *)((void*)data_ov052_021125a0[0].x))->Release();
  ((SharedFilePtr *)((void*)data_ov052_021125a0[1].x))->Release();
  return 1;
}
