//cpp
// @symbol _ZN10daPgDfdr_c8BehaviorEv
/* daPgDfdr_c::Behavior -- vtable slot 6. Real C++ method over the shared
 * header. Was an extern "C" free function over a raw `char *` with every field
 * reached by literal offset; converting it and naming the members is byte-exact
 * under the pinned 2004/b56.
 *
 * The two Animation::Advance calls are the same call on two different
 * sub-objects, which the raw offsets hid: `c + 0x370` is the Animation BASE of
 * mModelAnim (0x320 + 0x50, the multiple-inheritance offset include/ModelAnim.h
 * documents), and `c + 0x384` is mTextureSequence, whose Animation base sits at
 * offset 0. Spelling both as a base-pointer conversion is what makes that
 * readable -- and the compiler computes the same two addresses. */
#include "daPgDfdr_c.h"
#include "decl_Player.h"
#include "decl_common.h"

extern "C" {
extern void _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void* t, int a, int b);
extern void* _ZN8dActor_c13ClosestPlayerEv(void* c);
extern void _ZN9Animation7AdvanceEv(void* a);
extern void _ZN5dCc_c5ClearEv(void* a);
extern void _ZN5dCc_c6UpdateEv(void* a);
extern void func_ov027_02111994(char* c);
}

s32 daPgDfdr_c::Behavior()
{
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0);
    func_ov027_02111cfc((char*)this);
    if (_ZN6Player16IsInsideOfCannonEv(_ZN8dActor_c13ClosestPlayerEv(this))) {
        mFlags &= ~2;
    } else {
        mFlags |= 2;
    }
    _ZN9Animation7AdvanceEv(static_cast<Animation*>(&mModelAnim));
    _ZN9Animation7AdvanceEv(static_cast<Animation*>(&mTextureSequence));
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    func_ov027_02111994((char*)this);
    return 1;
}
