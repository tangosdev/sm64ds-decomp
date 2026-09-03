//cpp
// @symbol _ZN17daObjKm2_Nobiru_c8BehaviorEv
#include "daObjKm2_Nobiru_c.h"

/* SetScaleY's ROM symbol carries a by-value Fix12<int>. Its definition remains
 * at the measured class-parameter ABI wall, so this caller preserves the exact
 * extern-C entry point while using the real owned collider. */
extern "C" s32 func_0203aad0(dBgW_KcMbgSclY *);
extern "C" void _ZN14dBgW_KcMbgSclY9SetScaleYE5Fix12IiE(
    dBgW_KcMbgSclY *, s32);

int daObjKm2_Nobiru_c::Behavior()
{
    s32 scaleY = func_0203aad0(&mCollider);
    if (mGrowing) {
        _ZN14dBgW_KcMbgSclY9SetScaleYE5Fix12IiE(&mCollider, scaleY + 8);
        if (func_0203aad0(&mCollider) > 0x1000)
            mGrowing = 0;
    } else {
        _ZN14dBgW_KcMbgSclY9SetScaleYE5Fix12IiE(&mCollider, scaleY - 8);
        if (func_0203aad0(&mCollider) < 0x800)
            mGrowing = 1;
    }
    return 1;
}
