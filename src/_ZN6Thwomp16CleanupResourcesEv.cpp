//cpp
// @symbol _ZN6Thwomp16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Thwomp.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);

int Thwomp::CleanupResources()
{
    void **fp;
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMovingMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMovingMeshCollider);
    }
    fp = *(void***)((char *)&mFileTable);
    _ZN13SharedFilePtr7ReleaseEv(fp[0]);
    fp = *(void***)((char *)&mFileTable);
    _ZN13SharedFilePtr7ReleaseEv(fp[1]);
    fp = *(void***)((char *)&mFileTable);
    if (fp[3] != 0) {
        _ZN13SharedFilePtr7ReleaseEv(fp[3]);
    }
    return 1;
}
