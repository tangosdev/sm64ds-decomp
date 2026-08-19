//cpp
// @symbol _ZN12WithMeshClsn13SetGroundFlagEv
/* recovered: named members + shared header, real C++ method */
#include "WithMeshClsn.h"


void WithMeshClsn::SetGroundFlag()
{
    *(unsigned int *)((char *)&mFlags) |= 0x10;
}
