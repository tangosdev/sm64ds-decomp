//cpp
// @symbol _ZN9Animation8SetFlagsEi
/* recovered: named members + shared header, real C++ method */
#include "Animation.h"


void Animation::SetFlags(int flags)
{
  *(unsigned int*)((char*)&mFrameCountAndFlags)=(*(unsigned int*)((char*)&mFrameCountAndFlags)&0x3fffffff)|flags;
}
