//cpp
// @symbol _ZN13TTC_MovingBar16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TTC_MovingBar.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);

int TTC_MovingBar::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov065_0211d35c + (unsigned char)((char *)this)[0x31e]*0xc));
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov065_0211d360 + (unsigned char)((char *)this)[0x31e]*0xc));
  return 1;
}
