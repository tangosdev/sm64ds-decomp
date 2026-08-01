//cpp
// @symbol _ZN13FortressTower16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FortressTower.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov102_0214e188[];
extern char data_ov102_0214e18c[];

int FortressTower::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char *)&mMovingMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char *)&mMovingMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov102_0214e188 + (unsigned char)((char *)this)[0x31e]*0xc));
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov102_0214e18c + (unsigned char)((char *)this)[0x31e]*0xc));
  return 1;
}
