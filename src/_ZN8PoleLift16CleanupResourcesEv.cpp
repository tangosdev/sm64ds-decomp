//cpp
// @symbol _ZN8PoleLift16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PoleLift.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int G0[];

int PoleLift::CleanupResources()
{
    _ZN16MeshColliderBase7DisableEv((char *)&mCollider);
    _ZN13SharedFilePtr7ReleaseEv(G0);
    _ZN13SharedFilePtr7ReleaseEv(G1);
    return 1;
}
