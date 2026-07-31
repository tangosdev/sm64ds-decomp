//cpp
// @symbol _ZN14KnockDownPlank8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "KnockDownPlank.h"
struct C;
typedef void (C::*PMF)();
struct TabEnt { PMF pmf; };
extern TabEnt data_ov015_021149ec[];
extern "C" {
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *);
}
struct C {
    char pad[0x330];
    int idx;
};

int KnockDownPlank::Behavior()
{
    (((C *)this)->*(data_ov015_021149ec[((C *)this)->idx].pmf))();
    _ZN8Platform21UpdateModelPosAndRotYEv(((C *)this));
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((C *)this), 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(((C *)this));
    return 1;
}
