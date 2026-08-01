//cpp
// @symbol _ZN16RotatingCogSmall16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingCogSmall.h"
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov035_02112c78[];
extern char data_ov035_02112c70[];
extern char data_ov035_02112c60[];
}

int RotatingCogSmall::CleanupResources()
{
  if(mRotationState==0){
    if(_ZN16MeshColliderBase9IsEnabledEv((char *)&mMovingMeshCollider))
      _ZN16MeshColliderBase7DisableEv((char *)&mMovingMeshCollider);
    _ZN13SharedFilePtr7ReleaseEv(data_ov035_02112c78);
    _ZN13SharedFilePtr7ReleaseEv(data_ov056_02112c68);
  } else {
    int on = (unk_00c==0x79);
    if(on)
      _ZN13SharedFilePtr7ReleaseEv(data_ov035_02112c70);
    else
      _ZN13SharedFilePtr7ReleaseEv(data_ov035_02112c60);
  }
  return 1;
}
