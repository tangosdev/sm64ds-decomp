//cpp
// @symbol _ZN5Crate16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Crate.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov098_0213c4c8[];

int Crate::CleanupResources()
{
  int* f;
  if(_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
  f = 0;
  if(mActorID == 0xc2) f = data_ov098_0213c4c8;
  if(f){
    _ZN13SharedFilePtr7ReleaseEv((void*)f[0]);
    _ZN13SharedFilePtr7ReleaseEv((void*)f[1]);
  }
  return 1;
}
