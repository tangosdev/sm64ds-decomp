//cpp
// @symbol _ZN10SlidingIce16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "SlidingIce.h"
extern "C" void _ZN16MeshColliderBase7DisableEv(char*);
extern "C" void _ZN13SharedFilePtr7ReleaseEv(void*);
extern char func_ov030_02113be8[];
extern char data_ov027_02113be0[];

int SlidingIce::CleanupResources()
{
  unsigned char ok = (mActorID==0x5d);
  if(ok){ _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider); }
  _ZN13SharedFilePtr7ReleaseEv(func_ov030_02113be8);
  _ZN13SharedFilePtr7ReleaseEv(data_ov027_02113be0);
  return 1;
}
