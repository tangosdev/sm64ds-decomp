//cpp
// @symbol _ZN9SeesawBob16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SeesawBob.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"

int SeesawBob::CleanupResources()
{
  if(((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
    ((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov095_021374a0 + (unsigned char)((char *)this)[0x31e]*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov095_021374a4 + (unsigned char)((char *)this)[0x31e]*0xc)))->Release();
  return 1;
}
