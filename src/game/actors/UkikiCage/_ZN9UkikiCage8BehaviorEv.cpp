//cpp
// @symbol _ZN9UkikiCage8BehaviorEv
/* recovered: renamed to Class_Method, declarations from a shared header
 *
 * UkikiCage::Behavior -- vtable slot 6, ov030 0x02111384. Attributed by the
 * vtable: _ZTV9UkikiCage carries this address at slot 6, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043b24.
 *
 * Kept as a free function under the literal mangled name rather than a real
 * method. The old body cast `c` to `struct daObjHmBskt_c *` and read named
 * fields; that flat struct no longer exists under __cplusplus now that
 * include/UkikiCage.h declares a real class (its members are the base
 * dActor_c/dBgActor_c fields, not restated here), so the same offsets --
 * mAngleY/mHorzSpeed/mVertAccel at 0x8e/0x98/0x9c (include/dActor_c.h) and
 * this class's own mPosX/Y/Z and mStarActor (0x4dc) -- are read by hand offset
 * instead, unconverted from the pre-migration bytes. */
#include "decl_common.h"
extern "C" int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(char *c, int a, int b);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(char *c, char *clsn);
extern "C" int _ZN9UkikiCage8BehaviorEv(char *c) {
    volatile int dummy[4];
    (void)&dummy;
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0);
    *(short *)(c + 0x8e) = *(short *)(c + 0x8e) + *(int *)(c + 0x98);
    if (*(int *)(c + 0x9c) != 0) {
        *(int *)(((int)c + 0xb0)) = *(int *)(((int)c + 0xb0)) & ~1;
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char*)0);
        func_ov030_0211124c(c, c+0x320);
        int a0 = *(int *)(c + 0x60);
        int a1 = *(int *)(c + 0x64);
        int m = *(int *)(c + 0x4dc);
        int a2 = *(int *)(c + 0x5c);
        *(int*)(m + 0x5c) = a2;
        *(int*)(m + 0x60) = a0 + 0x3c000;
        *(int*)(m + 0x64) = a1;
    }
    return 1;
}
