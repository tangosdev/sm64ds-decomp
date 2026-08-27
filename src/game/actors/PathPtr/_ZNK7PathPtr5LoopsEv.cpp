//cpp
// @symbol _ZNK7PathPtr5LoopsEv
#include "PathPtr.h"

unsigned int PathPtr::Loops() const
{
    return def->flags & 0x80;
}
