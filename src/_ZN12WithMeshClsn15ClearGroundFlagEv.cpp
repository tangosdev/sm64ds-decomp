//cpp
// @symbol _ZN12WithMeshClsn15ClearGroundFlagEv
/* recovered: named members + shared header, real C++ method */
#include "WithMeshClsn.h"


void WithMeshClsn::ClearGroundFlag()
{
    *(unsigned int *)(((long long)(int)((char *)&mFlags))) &= ~0x10;
}
