//cpp
// @symbol _ZNK7PathPtr5LoopsEv
/* recovered: named members + shared header, real C++ method */
#include "PathPtr.h"


unsigned int PathPtr::Loops() const
{
  return (*((unsigned char **)this))[5] & 0x80;
}
