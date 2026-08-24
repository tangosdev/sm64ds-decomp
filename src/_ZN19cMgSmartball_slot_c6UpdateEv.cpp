//cpp
/* Slot 1. First loop draws three copies of one sprite in a row (index i is
 * computed but never read -- only the running x offset matters). Second
 * loop, over the same three mReelOffset[i] pairs, draws a stack of four more
 * sprites per element with descending y. Kept byte-for-byte identical to
 * the pre-migration matched source, including the `if (1) {}` no-op, the
 * (x - 0x18) & 0xFFFFFFFFFFFFFFFF mask, and the goto-based second loop --
 * only the raw offset accesses became real member access. */
#include "cMgSmartball_slot_c.h"

extern "C" void *data_ov006_021379dc;
extern "C" void *data_ov006_021379e8;
extern "C" void *data_ov006_02137a00;
extern "C" void *data_ov006_021379f4;
extern "C" void func_ov004_020afdd0(void *, int, int, int, int);

inline int inline_fn(int arg0, int arg1)
{
    return arg0 + arg1;
}

void cMgSmartball_slot_c::Update()
{
    int i;
    int x;
    int xo;
    for (i = 0, x = 0; i < 3; i++, x += 0x18) {
        if (1) {
        }
        func_ov004_020afdd0(data_ov006_021379dc, inline_fn(mCurrent0 >> 12, x - 0x18), mCurrent1 >> 12, -1, 2);
    }

    i = 0;
    x = 0;
second_loop:
    {
        xo = (x - 0x18) & 0xFFFFFFFFFFFFFFFF;
        func_ov004_020afdd0(data_ov006_021379e8, inline_fn(inline_fn(mCurrent0 >> 12, mReelOffset[i].x >> 12), xo), inline_fn(mCurrent1 >> 12, mReelOffset[i].y >> 12), -1, 3);
        func_ov004_020afdd0(data_ov006_02137a00, inline_fn(inline_fn(mCurrent0 >> 12, mReelOffset[i].x >> 12), xo), inline_fn(mCurrent1 >> 12, mReelOffset[i].y >> 12) - 0x10, -1, 3);
        func_ov004_020afdd0(data_ov006_021379f4, inline_fn(inline_fn(mCurrent0 >> 12, mReelOffset[i].x >> 12), xo), inline_fn(mCurrent1 >> 12, mReelOffset[i].y >> 12) - 0x20, -1, 3);
        func_ov004_020afdd0(data_ov006_021379e8, inline_fn(inline_fn(mCurrent0 >> 12, mReelOffset[i].x >> 12), xo), inline_fn(mCurrent1 >> 12, mReelOffset[i].y >> 12) - 0x30, -1, 3);
        x += 0x18;
        i++;
        if (i < 3)
            goto second_loop;
    }
}
