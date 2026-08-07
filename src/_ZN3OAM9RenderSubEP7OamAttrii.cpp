//cpp
// @symbol _ZN3OAM9RenderSubEP7OamAttrii
#include "OAM.h"

/* NOTE ON THE LOCAL DEFINITION BELOW.
 * It stays, against plan section 6's "no local struct body" rule, and OAM.h
 * forward-declares OamAttr rather than defining it. Centralising the type was tried
 * and reverted: the tree holds three incompatible spellings of this 8-byte type --
 * this one, LoadAffineParams' `a, b, c, param`, and Render's bitfield decomposition
 * -- and a shared include/OamAttr.h broke the ROM build on the other two.
 * Reconciling them is its own change. See include/OAM.h. */
struct OamAttr { u16 attr0, attr1, attr2, attr3; };

/* OAM::RenderSub(OamAttr* data, s32 x, s32 y) at 0x0202194c -- static, no `this`.
 *
 * The short form of the sub-screen sprite call: renders unscaled, unrotated, with
 * the palette and priority already in the attribute words. The defaults it supplies
 * are what "no transform" means to the hardware -- 0x1000 is 1.0 in 20.12 for both
 * scales, and -1 in palette/priority/mode means "leave the attribute word alone".
 *
 * THE TYPE IS `OamAttr`, NOT `OamAttri`. This file used to declare a local
 * `struct OamAttri`, and the mangled name reads `P7OamAttrii` -- pointer to a
 * SEVEN-character class, then two ints. The extra `i` belongs to the parameter
 * list, not the type name. That was invisible while the symbol was hand-spelled;
 * the moment the compiler mangles it, `OamAttri` produces `P8OamAttriii` and the
 * function vanishes from the object. See include/OamAttr.h.
 *
 * The full Render is NOT declared as a method and must not be: its scale parameters
 * are by-value `Fix12<int>`, and mwccarm homes r0-r3 to the stack for a by-value
 * class parameter (+0x14) on every sweep version at every optimisation level. It
 * stays an `extern "C"` free function taking scalars -- runbook section 7.
 */
extern "C" void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    int sub, OamAttr *data, s32 x, s32 y,
    s32 palette, s32 priority, s32 scaleX, s32 scaleY,
    s32 rotation, s32 mode);

void OAM::RenderSub(OamAttr *data, s32 x, s32 y)
{
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        1, data, x, y,
        -1, -1,
        0x1000, 0x1000,
        0, -1);
}
