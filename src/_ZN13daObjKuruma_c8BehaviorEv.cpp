//cpp
// @symbol _ZN13daObjKuruma_c8BehaviorEv
#include "daObjKuruma_c.h"
/* daObjKuruma_c::Behavior -- vtable slot 6.
 *
 * Attributed by the vtable, not by a `recovered name:` comment: the old file
 * carried none at all, only the func_ov002_ address. include/daObjKuruma_c.h's
 * own banner already names 0x020b6920 as this class's Behavior, from the ROM's
 * table at ov002 0x02109278.
 *
 * Two of the three calls are dBgActor_c's own non-virtual members, so they are
 * spelled as calls now rather than through `extern "C"` declarations of the
 * mangled symbols -- the compiler emits the identical direct `bl` either way.
 * IsClsnInRange stays a mangled extern because include/dBgActor_c.h does not
 * declare it and that header belongs to the whole dBgActor_c family, not to this
 * slice; naming it there is its own change.
 *
 * `this` is not adjusted anywhere below: daObjKuruma_c has a single base at
 * subobject offset 0 (__si_class_type_info, per the banner), so the void* the
 * ROM passes in r0 and `this` are the same address. */

extern "C" int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *thiz, int x, int z);

s32 daObjKuruma_c::Behavior()
{
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}
