//cpp
// @symbol _ZN9Animation8FinishedEv
/* recovered: named members + shared header, real C++ method */
#include "Animation.h"


int Animation::Finished()
{
  unsigned int f=*(unsigned int*)((char*)&mFrameCountAndFlags);
  int cur=*(int*)((char*)&unk_008);
  return cur>=(int)((f&0x3fffffff)-1);
}
