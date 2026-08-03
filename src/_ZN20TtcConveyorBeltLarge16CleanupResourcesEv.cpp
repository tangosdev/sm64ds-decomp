//cpp
// @symbol _ZN20TtcConveyorBeltLarge16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TtcConveyorBeltLarge.h"
#include "SharedFilePtr.h"
extern char data_ov065_0211d194[];
extern char data_ov065_0211d198[];

int TtcConveyorBeltLarge::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
  ((SharedFilePtr *)(*(void**)(data_ov065_0211d194 + (unsigned char)((char *)this)[0x39e]*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov065_0211d198 + (unsigned char)((char *)this)[0x39e]*0xc)))->Release();
  return 1;
}
