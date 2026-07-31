//cpp
// @symbol _ZN8IceSheet8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "IceSheet.h"
extern "C" {
int _ZN16MeshColliderBase9IsEnabledEv(void*);
void _ZN16MeshColliderBase6EnableEP5Actor(void*, void*);
}

int IceSheet::Behavior()
{
  if (!_ZN16MeshColliderBase9IsEnabledEv((char*)&mMovingMeshCollider))
    _ZN16MeshColliderBase6EnableEP5Actor(((char*)this)+0x124, ((char*)this));
  return 1;
}
