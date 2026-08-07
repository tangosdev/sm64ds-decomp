//cpp
// @symbol _ZN13PeachPainting8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "PeachPainting.h"
struct Actor { int DistToCPlayer(); };
namespace cstd { int fdiv(int a, int b); }
/* ModelBase is the real class (include/ModelBase.h), reached through
   PeachPainting.h -> Model.h. Its ApplyOpacity is declared there with one
   argument; the ROM's takes two, so the call keeps the mangled spelling:
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN9ModelBase12ApplyOpacityEj(void *, unsigned int o, int x);


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
    _ZN9ModelBase12ApplyOpacityEj((ModelBase *)((char *)&mModel), mOpacity, 1);
    return 1;
}
