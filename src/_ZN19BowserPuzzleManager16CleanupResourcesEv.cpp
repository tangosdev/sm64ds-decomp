//cpp
// @symbol _ZN19BowserPuzzleManager16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserPuzzleManager.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern void *data_ov064_0211adc8[];

int BowserPuzzleManager::CleanupResources()
{
    unsigned char idx;
    _ZN16MeshColliderBase7DisableEv((char *)&mMovingMeshCollider);
    idx = *(unsigned char *)((char *)&unk_337);
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211adc8[idx]);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov075_0211c800);
    return 1;
}
