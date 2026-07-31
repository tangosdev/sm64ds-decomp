//cpp
// @symbol _ZN12WithMeshClsn13SetLimMovFlagEv
/* recovered: named members + shared header, real C++ method */
#include "WithMeshClsn.h"


void WithMeshClsn::SetLimMovFlag()
{
    *(unsigned int *)(((long long)(int)((char *)&mFlags))) |= 0x80;
}
