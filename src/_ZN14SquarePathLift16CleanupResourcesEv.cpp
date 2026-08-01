//cpp
// @symbol _ZN14SquarePathLift16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "SquarePathLift.h"
extern "C" {
struct SFP{int x;};
extern SFP data_ov052_021125a0[2];
int _ZN16MeshColliderBase9IsEnabledEv(void*);
void _ZN16MeshColliderBase7DisableEv(void*);
void _ZN13SharedFilePtr7ReleaseEv(void*);
}

int SquarePathLift::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char*)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv((void*)data_ov052_021125a0[0].x);
  _ZN13SharedFilePtr7ReleaseEv((void*)data_ov052_021125a0[1].x);
  return 1;
}
