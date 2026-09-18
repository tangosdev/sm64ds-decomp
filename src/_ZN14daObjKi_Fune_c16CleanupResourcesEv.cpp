//cpp
// @symbol _ZN14daObjKi_Fune_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjKi_Fune_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" {
}

int daObjKi_Fune_c::CleanupResources()
{
  if(((dBgW *)((char*)&mMeshCollider))->IsEnabled())
    ((dBgW *)((char*)&mMeshCollider))->Disable();
  ((SharedFilePtr *)(data_ov016_021136e4[mModelIndex]))->Release();
  ((SharedFilePtr *)(data_ov016_021136dc[mModelIndex]))->Release();
  return 1;
}
