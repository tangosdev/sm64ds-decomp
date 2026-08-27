//cpp
// @symbol _ZN14ArrowSignRight16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ArrowSignRight.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern char data_ov098_0213c380[];

int ArrowSignRight::CleanupResources()
{
  if(((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
    ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov098_0213c380 + (unsigned char)((char *)this)[0x37c]*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov098_0213c384 + (unsigned char)((char *)this)[0x37c]*0xc)))->Release();
  return 1;
}
