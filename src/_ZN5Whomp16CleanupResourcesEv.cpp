//cpp
// @symbol _ZN5Whomp16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Whomp.h"
#include "SharedFilePtr.h"
extern void data_ov079_02128168(void);
extern void data_ov079_02128178(void);
extern void data_ov079_02128170(void);
extern void *data_ov079_021275ec[];

int Whomp::CleanupResources()
{
  int i;
  if(_ZN16MeshColliderBase9IsEnabledEv((unsigned char *)&mMovingMeshCollider))
    _ZN16MeshColliderBase7DisableEv((unsigned char *)&mMovingMeshCollider);
  ((SharedFilePtr *)(data_ov079_02127bf0[((unsigned char *)this)[0x414]]))->Release();
  if(((unsigned char *)this)[0x414]){
    ((SharedFilePtr *)((void*)&data_ov079_02128168))->Release();
    for(i=0;i<6;i++)
      ((SharedFilePtr *)(data_ov079_02127600[i]))->Release();
    ((SharedFilePtr *)((void*)&data_ov079_02128178))->Release();
  } else {
    ((SharedFilePtr *)((void*)&data_ov079_02128170))->Release();
    for(i=0;i<5;i++)
      ((SharedFilePtr *)(data_ov079_021275ec[i]))->Release();
  }
  return 1;
}
