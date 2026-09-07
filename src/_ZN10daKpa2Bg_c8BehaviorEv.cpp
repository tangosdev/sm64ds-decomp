//cpp
// @symbol _ZN10daKpa2Bg_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daKpa2Bg_c.h"

int daKpa2Bg_c::Behavior()
{
    short *a = (short *)(((int)((char *)this) + 0x8c));
    short *b = (short *)(((int)((char *)this) + 0x8e));

    *a = *a + mAngleXSpeed;
    *b = *b + mAngleYSpeed;

    {
        short *d = (short *)(((int)((char *)this) + 0x90));
        *d = *d + mAngleZSpeed;
    }

    func_ov060_02117ae0(((char *)this));
    func_ov060_02117a64(((char *)this));
    return 1;
}
