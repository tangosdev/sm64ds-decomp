//cpp
// @symbol _ZN13TTC_MovingBar16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TTC_MovingBar.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"

int TTC_MovingBar::CleanupResources()
{
  if(((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled())
    ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov065_0211d35c + (unsigned char)((char *)this)[0x31e]*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov065_0211d360 + (unsigned char)((char *)this)[0x31e]*0xc)))->Release();
  return 1;
}
