//cpp
// @symbol _ZN9MovingBar8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "MovingBar.h"
struct C;
typedef void (C::*PMF)();
struct TabEnt { PMF pmf; };
extern TabEnt data_ov015_021149ec[];
extern "C" {
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);
}
struct C {
    char pad[0x330];
    int idx;
};

int MovingBar::Behavior()
{
    (((C *)this)->*(data_ov015_021149ec[mState].pmf))();
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((C *)this));
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((C *)this), 0, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((C *)this));
    return 1;
}
