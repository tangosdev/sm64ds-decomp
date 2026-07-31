//cpp
// @symbol _ZN14KnockDownPlank16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "KnockDownPlank.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov015_02114534[];

int KnockDownPlank::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider))
        _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    _ZN13SharedFilePtr7ReleaseEv(*(void **)(data_ov015_02114534 + mVariant * 0xc));
    _ZN13SharedFilePtr7ReleaseEv(*(void **)(data_ov015_02114538 + mVariant * 0xc));
    return 1;
}
