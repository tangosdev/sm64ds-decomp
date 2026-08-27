//cpp
// @symbol _ZN8SignPost15OnGroundPoundedER8dActor_c
#include "SignPost.h"
#include "dActor_c.h"

/* SignPost::OnGroundPounded -- vtable slot 21, ov002 0x020bb27c.
 * reloc: _ZTV8SignPost+0x54 -> 0x020bb27c, _ZTV10dBgActor_c+0x54 ->
 * 0x02010148 (different, real override).
 *
 * include/dActor_c.h's own slot 21 supplies the signature -- `void`, the
 * tree-wide fix from Stump::OnGroundPounded (a64045669).
 *
 * mPoundsLeft/mPoundCooldown/mRespawnDelay are this class's own fields (include/SignPost.h);
 * mPoundCooldown and mRespawnDelay were undescribed padding until this method's body
 * proved they are read/written. `&other + 0x703` reads past dActor_c's own
 * span -- same raw-offset reading src/_ZN5Stump15OnGroundPoundedER8dActor_c.cpp
 * records for its own slot 21. */
extern "C" void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *v);
extern "C" void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *c);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *c);

void SignPost::OnGroundPounded(dActor_c &other)
{
    if (mPoundsLeft == 0) return;
    if (mPoundCooldown != 0) return;
    _ZN5Sound9PlayBank3EjRK7Vector3(0x62, &mCamSpacePosX);
    if (other.param1 == 2 || *(unsigned char *)((char *)&other + 0x703) != 0) {
        mPosY -= (mPoundsLeft * 0x2d) << 12;
        mPoundsLeft = 0;
        _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
        mRespawnDelay = 0x1e;
    } else {
        mPosY -= 0x2d000;
        mPoundsLeft -= 1;
        _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
        mPoundCooldown = 0xf;
        mRespawnDelay = 0x1e;
    }
}
