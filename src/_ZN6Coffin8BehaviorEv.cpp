//cpp
// @symbol _ZN6Coffin8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Coffin.h"
extern "C" {
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char *self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(char *self);
}

int Coffin::Behavior()
{
    char *stateField;

    if ((*(int *)(stateField = ((char *)this) + 8) & 0xff) == 1) {
        func_ov071_02122080(((char *)this));
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0)) {
            _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
        }
        return 1;
    }

    {
        unsigned short *timer =
            (unsigned short *)(((int)((char *)this) + 0x328) & 0xFFFFFFFFFFFFFFFFLL);
        *timer = *timer + 1;
    }
    func_ov071_02122414(((char *)this));
    func_ov071_02122080(((char *)this));
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0)) {
        _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
    }
    return 1;
}
