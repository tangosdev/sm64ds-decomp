#include "types.h"
extern void _ZN4cstd16reciprocal_asyncE5Fix12IiE(s32 x);
extern void func_02053008(s32 x);
extern s64 _ZN4cstd11ldiv_resultEv(void);

extern volatile u16 SQRTCNT;     /* 0x040002b0 */
extern volatile s32 SQRT_RESULT; /* 0x040002b4 */

s32 func_02053130(s32 r0)
{
    s32 r4 = r0;
    s64 divres;
    volatile u16 *sqrtcnt;
    s32 r2;

    if (r4 <= 0)
        goto ret_zero;

    _ZN4cstd16reciprocal_asyncE5Fix12IiE(r4);
    func_02053008(r4);
    divres = _ZN4cstd11ldiv_resultEv();

    sqrtcnt = &SQRTCNT;
    while (*sqrtcnt & 0x8000)
        ;

    r2 = 0;
    return (s32)((divres * (s64)SQRT_RESULT + ((s64)r2 + (s64)0x200) * (s64)(1LL << 32)) >> (32 + 10));

ret_zero:
    return 0;
}