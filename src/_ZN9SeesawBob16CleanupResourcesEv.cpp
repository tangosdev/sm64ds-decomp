//cpp
// @symbol _ZN9SeesawBob16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SeesawBob.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);

int SeesawBob::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov095_021374a0 + (unsigned char)((char *)this)[0x31e]*0xc));
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov095_021374a4 + (unsigned char)((char *)this)[0x31e]*0xc));
  return 1;
}
