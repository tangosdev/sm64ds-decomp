//cpp
// @symbol _ZN3OAM9RenderSubEP7OamAttriiii
#include "OAM.h"

#include "OamAttr.h"

/* OAM::RenderSub(OamAttr* data, s32 x, s32 y, s32 palette, s32 priority)
 * at 0x0202199c -- static, no `this`.
 *
 * The long form: same as the three-argument overload but with palette and priority
 * given explicitly rather than left as -1. Everything else is still the identity
 * transform -- 0x1000 scale in both axes (1.0 in 20.12), no rotation, mode -1.
 *
 * `P7OamAttriiii` is pointer-to-`OamAttr` followed by FOUR ints; the class name is
 * seven characters. Local `struct OamAttri` retired in favour of
 * include/OamAttr.h -- see the sibling overload for why that `i` mattered.
 *
 * Render stays an `extern "C"` free function -- by-value `Fix12<int>` scale
 * parameters, runbook section 7.
 */
extern "C" void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    int sub, OamAttr *data, s32 x, s32 y,
    s32 palette, s32 priority, s32 scaleX, s32 scaleY,
    s32 rotation, s32 mode);

void OAM::RenderSub(OamAttr *data, s32 x, s32 y, s32 palette, s32 priority)
{
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        1, data, x, y,
        palette, priority,
        0x1000, 0x1000,
        0, -1);
}
