//cpp
// @symbol _ZNK7PathPtr8NumNodesEv
/* recovered: named members + shared header, real C++ method */
#include "PathPtr.h"


unsigned int PathPtr::NumNodes() const
{
  return (*((unsigned char **)this))[2];
}
