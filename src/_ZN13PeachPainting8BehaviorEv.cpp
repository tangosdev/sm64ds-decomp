//cpp
// @symbol _ZN13PeachPainting8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "PeachPainting.h"
struct Actor { int DistToCPlayer(); };
namespace cstd { int fdiv(int a, int b); }
struct ModelBase { void ApplyOpacity(unsigned int o, int x); };

int PeachPainting::Behavior()
{
    int d = ((Actor *)((char *)this))->DistToCPlayer();
    if (d >= 0xe10000) {
        mOpacity = 0xff;
    } else if (d <= 0xbf4000) {
        mOpacity = 0;
    } else {
        int q = cstd::fdiv(d - 0xbf4000, 0x21c000);
        int o = (int)(((long long)q * 0xff + 0x800) >> 12);
        mOpacity = (unsigned char)(o >> 3);
    }
    ((ModelBase *)((char *)&mModel))->ApplyOpacity(mOpacity, 1);
    return 1;
}
