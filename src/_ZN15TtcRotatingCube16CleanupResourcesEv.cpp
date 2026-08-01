//cpp
// @symbol _ZN15TtcRotatingCube16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TtcRotatingCube.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void* self);
struct E { void* p; char pad[8]; };
extern struct E data_ov065_0211cfd0[];
extern struct E data_ov065_0211cfd4[];

int TtcRotatingCube::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char*)&mMovingMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char*)&mMovingMeshCollider);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211c0a8[unk_377]);
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211cfd0[unk_377].p);
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211cfd4[unk_377].p);
    return 1;
}
