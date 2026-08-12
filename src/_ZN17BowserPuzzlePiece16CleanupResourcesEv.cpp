//cpp
// @symbol _ZN17BowserPuzzlePiece16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserPuzzlePiece.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern void *data_ov064_0211adc8[];
extern void *data_ov064_0211c800;

int BowserPuzzlePiece::CleanupResources()
{
    unsigned char idx;
    ((MeshColliderBase *)((char *)&mMovingMeshCollider))->Disable();
    idx = *(unsigned char *)((char *)&unk_337);
    ((SharedFilePtr *)(data_ov064_0211adc8[idx]))->Release();
    ((SharedFilePtr *)(&data_ov064_0211c800))->Release();
    return 1;
}
