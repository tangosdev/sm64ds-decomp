//cpp
#include "Sound.h"
#include "dBgActor_c.h"
// @symbol _ZN8SignPost15OnGroundPoundedER8dActor_c
#include "SignPost.h"
#include "dActor_c.h"

/* SignPost::OnGroundPounded -- vtable slot 21, ov002 0x020bb27c.
 * reloc: _ZTV8SignPost+0x54 -> 0x020bb27c, _ZTV10dBgActor_c+0x54 ->
 * 0x02010148 (different, real override).
 *
 * include/dActor_c.h's own slot 21 supplies the signature -- `void`, the
 * tree-wide fix from daObjPile_c::OnGroundPounded (36bc6d1df).
 *
 * mPoundsLeft/mPoundCooldown/mRespawnDelay are this class's own fields (include/SignPost.h);
 * mPoundCooldown and mRespawnDelay were undescribed padding until this method's body
 * proved they are read/written. `&other + 0x703` reads past dActor_c's own
 * span -- same raw-offset reading daObjPile_c::OnGroundPounded records
 * for its own slot 21. */

void SignPost::OnGroundPounded(dActor_c &other)
{
    if (mPoundsLeft == 0) return;
    if (mPoundCooldown != 0) return;
    Sound::PlayBank3(0x62, *(const Vector3 *)&mCamSpacePosX);
    if (other.param1 == 2 || *(unsigned char *)((char *)&other + 0x703) != 0) {
        mPosY -= (mPoundsLeft * 0x2d) << 12;
        mPoundsLeft = 0;
        ((dBgActor_c *)this)->UpdateModelPosAndRotY();
        ((dBgActor_c *)this)->UpdateClsnPosAndRot();
        mRespawnDelay = 0x1e;
    } else {
        mPosY -= 0x2d000;
        mPoundsLeft -= 1;
        ((dBgActor_c *)this)->UpdateModelPosAndRotY();
        ((dBgActor_c *)this)->UpdateClsnPosAndRot();
        mPoundCooldown = 0xf;
        mRespawnDelay = 0x1e;
    }
}
