//cpp
// @symbol _ZN12WithMeshClsn22ClearJustHitGroundFlagEv
/* recovered: named members + shared header, real C++ method */
#include "WithMeshClsn.h"


void WithMeshClsn::ClearJustHitGroundFlag()
{
    *(unsigned int *)(((long long)(int)((char *)&mFlags))) &= ~0x20;
}
