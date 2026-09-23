//cpp
// @symbol _ZN16daObjCtMecha10_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjCtMecha10_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" {
extern char data_ov035_02112c78[];
extern char data_ov035_02112c70[];
extern char data_ov035_02112c60[];
}

int daObjCtMecha10_c::CleanupResources()
{
  if(mRotationState==0){
    if(((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
      ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    ((SharedFilePtr *)(data_ov035_02112c78))->Release();
    ((SharedFilePtr *)(data_ov035_02112c68))->Release();
  } else {
    int on = (actorID==0x79);
    if(on)
      ((SharedFilePtr *)(data_ov035_02112c70))->Release();
    else
      ((SharedFilePtr *)(data_ov035_02112c60))->Release();
  }
  return 1;
}
