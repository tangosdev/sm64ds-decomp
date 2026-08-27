//cpp
// @symbol _ZN5Crate16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Crate.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov098_0213c4c8[];

int Crate::CleanupResources()
{
  int* f;
  if(((dBgW *)((char *)&mMeshCollider))->IsEnabled())
    ((dBgW *)((char *)&mMeshCollider))->Disable();
  f = 0;
  if(actorID == 0xc2) f = data_ov098_0213c4c8;
  if(f){
    ((SharedFilePtr *)((void*)f[0]))->Release();
    ((SharedFilePtr *)((void*)f[1]))->Release();
  }
  return 1;
}
