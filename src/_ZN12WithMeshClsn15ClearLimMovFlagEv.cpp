//cpp
// @symbol _ZN12WithMeshClsn15ClearLimMovFlagEv
/* recovered: named members + shared header, real C++ method */
#include "WithMeshClsn.h"


void WithMeshClsn::ClearLimMovFlag()
{
    *(unsigned int *)((char *)&mFlags) &= ~0x80;
}
