//cpp
// @symbol _ZN13CLPS_BlockRefaSER10CLPS_Block
/* The value lifetime is ROM-proven; the wrapper spelling is inferred. */
#include "CLPS_BlockRef.h"

CLPS_BlockRef &CLPS_BlockRef::operator=(CLPS_Block &block)
{
    ptr = &block;
    return *this;
}
