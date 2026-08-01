//cpp
// @symbol _ZN14ArrowSignRight16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ArrowSignRight.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov098_0213c380[];

int ArrowSignRight::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov098_0213c380 + (unsigned char)((char *)this)[0x37c]*0xc));
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov098_0213c384 + (unsigned char)((char *)this)[0x37c]*0xc));
  return 1;
}
