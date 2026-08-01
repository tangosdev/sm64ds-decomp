//cpp
// @symbol _ZN15FaderBrightness11AdvanceFadeEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FaderBrightness.h"

extern "C" {
void func_02053a90(u16* reg, int value);
void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 len);
void _ZN2GX10LoadBGPlttEPKvjj(const void* src, u32 offset, u32 len);
void _ZN3GXS10LoadBGPlttEPKvjj(const void* src, u32 offset, u32 len);
}

/* One frame of the brightness fade. Nothing reaches the hardware unless the
   interpolator actually moved, so a settled fade costs two loads. 0x400006c and
   0x400106c are MASTER_BRIGHT for the main and sub engines; the level is negated
   and rescaled from 20.12 into the register`s 5-bit field. Past full black the
   backdrop palette is reloaded as well. */
void FaderBrightness::AdvanceFade()
{
    Fix12i before = currInterp;
    int level;

    AdvanceInterp();
    if (currInterp == before)
        return;

    level = -(currInterp << 4) >> 0xc;
    func_02053a90((u16*)0x400006c, level);
    func_02053a90((u16*)0x400106c, level);
    if (level > -0x10)
        return;

    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_0209d3ac, 2);
    _ZN2GX10LoadBGPlttEPKvjj(data_0209d3ac, 0, 2);
    _ZN3GXS10LoadBGPlttEPKvjj(data_0209d3ac, 0, 2);
}
