//cpp
// @symbol _ZN13daObjCasket_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjCasket_c.h"
extern "C" {
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(char *self, int a, int b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(char *self);
}

int daObjCasket_c::Behavior()
{
    char *stateField;

    if ((*(int *)(stateField = ((char *)this) + 8) & 0xff) == 1) {
        func_ov071_02122080(((char *)this));
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0)) {
            _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));
        }
        return 1;
    }

    {
        unsigned short *timer =
            (unsigned short *)(((int)((char *)this) + 0x328));
        *timer = *timer + 1;
    }
    func_ov071_02122414(((char *)this));
    func_ov071_02122080(((char *)this));
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0)) {
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));
    }
    return 1;
}
