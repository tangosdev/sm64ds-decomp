//cpp
// @symbol _ZNK9Animation13GetFrameCountEv
/* recovered: named members + shared header, real C++ method */
#include "Animation.h"


unsigned int Animation::GetFrameCount() const
{
  unsigned int v=*(unsigned int*)((char*)&mFrameCountAndFlags);
  return ((v&0x3fffffff)<<4)>>16;
}
