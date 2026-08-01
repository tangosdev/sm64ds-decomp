//cpp
// @symbol _ZN15ChainChompFence8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "ChainChompFence.h"
extern "C" {
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
}

int ChainChompFence::Behavior()
{
    if (*(unsigned char*)((char*)&unk_31e) != 0) return 1;
    _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((void *)this), 0, 0);
    return 1;
}
