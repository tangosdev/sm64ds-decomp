//cpp
// @symbol _ZN13BigBrickBlock16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BigBrickBlock.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern char data_ov002_02108ab0[];
extern char data_ov002_02108ab4[];

int BigBrickBlock::CleanupResources()
{
  if(((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
    ((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov002_02108ab0 + (unsigned char)((char *)this)[0x32c]*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov002_02108ab4 + (unsigned char)((char *)this)[0x32c]*0xc)))->Release();
  return 1;
}
