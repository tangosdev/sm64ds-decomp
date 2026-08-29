//cpp
// @symbol _ZN9fLiNdBa_cD1Ev
#include "fListNode.h"

extern "C" fLiNdBa_c *data_020a4b68;

fLiNdBa_c::~fLiNdBa_c()
{
    if (data_020a4b68 == this)
        data_020a4b68 = 0;
}
