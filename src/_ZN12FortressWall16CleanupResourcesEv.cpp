//cpp
// @symbol _ZN12FortressWall16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FortressWall.h"
#include "SharedFilePtr.h"
extern char data_ov079_02128058[];
extern char data_ov079_0212805c[];

int FortressWall::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
  ((SharedFilePtr *)(*(void**)(data_ov079_02128058 + (unsigned char)((char *)this)[0x31e]*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov079_0212805c + (unsigned char)((char *)this)[0x31e]*0xc)))->Release();
  return 1;
}
