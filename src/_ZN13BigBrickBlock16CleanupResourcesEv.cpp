//cpp
// @symbol _ZN13BigBrickBlock16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BigBrickBlock.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov002_02108ab0[];
extern char data_ov002_02108ab4[];

int BigBrickBlock::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov002_02108ab0 + (unsigned char)((char *)this)[0x32c]*0xc));
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov002_02108ab4 + (unsigned char)((char *)this)[0x32c]*0xc));
  return 1;
}
