//cpp
// @symbol _ZN12WithMeshClsn19ClearAllGroundFlagsEv
/* recovered: named members + shared header, real C++ method */
#include "WithMeshClsn.h"


void WithMeshClsn::ClearAllGroundFlags()
{
    *(unsigned int *)((char *)&mFlags) &= ~0x70;
}
