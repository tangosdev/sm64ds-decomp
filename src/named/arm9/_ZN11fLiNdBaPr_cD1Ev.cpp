//cpp
// @symbol _ZN11fLiNdBaPr_cD1Ev
#include "fListNode.h"

extern "C" void *data_020a4b68;

fLiNdBaPr_c::~fLiNdBaPr_c()
{
    if (data_020a4b68 == this)
        data_020a4b68 = 0;
}
