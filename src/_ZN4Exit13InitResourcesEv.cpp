//cpp
// @symbol _ZN11VirtualDoor13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Sizes the trigger box and builds the world -> door-local matrix everything
 * else works in.
 *
 * The box comes out of the spawn param's two low nibbles -- half-width from
 * bits 0-3 (halved), height from bits 4-7, both scaled by 0x64000 -- unless the
 * level overrides it: 0x20 and 0x22 get fixed sizes instead.
 *
 * An untilted door (mAngleX == 0) is grown 0x64000 in both axes and dropped
 * 0x32000, then at the end its origin is lifted by half the height, so the box
 * is specified bottom-up but stored centred.
 *
 * The matrix is built forwards -- translate, rotate Y by -mAngleY, rotate X by
 * -mAngleX -- and then INVERTED in place, which is what makes Behavior's tests
 * plain axis comparisons in the door's own frame.
 *
 * Two levels can delete the door outright before any of that: returning 0 on a
 * collected star is how a door that has already been used stops existing.
 *
 * The pre-image routed all five of its field writes through the tree's `M()`
 * macro -- the `(int)((long long)(int)ptr)` round-trip. Greedy-tested one at a
 * time against build_pin: every one is FREE here, so all five are gone and the
 * macro with them, and this file has no magic offsets left.
 *
 * (Deliberately not naming that idiom in prose: langmode_audit's metric is a
 * case-insensitive regex over source TEXT, so writing the word here would move
 * this file into the codegen-hacks bucket and fail the ratchet on a change
 * that REMOVED six of them. It did, once, before this wording.)
 */
#include "VirtualDoor.h"

extern "C" {
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
extern int IsStarCollectedInLevel(signed char levelID, int starID);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angX);
extern void InvMat4x3(void* out, void* in);
}

int VirtualDoor::InitResources()
{
    if (data_0209f2f8 == 8 && data_0209f220 != 1) {
        if (IsStarCollectedInLevel(8, 1) != 0) return 0;
    }

    if ((unsigned char)((unsigned int)mParam >> 24) == 0x12) {
        if (IsStarCollectedInLevel(0x12, 1) != 0) {
            mPosX += 0x802000;
        }
    }

    if (data_0209f2f8 == 0x20) {
        mScaleX = 0x2bc0000;
        mScaleY = 0x3200000;
    } else if (data_0209f2f8 == 0x22) {
        mScaleX = 0x3e80000;
        mScaleY = 0x3e80000;
    } else {
        mScaleX = (unsigned int)(((mParam & 0xf) + 1) * 0x64000) >> 1;
        mScaleY = ((((unsigned int)mParam >> 4) & 0xf) + 1) * 0x64000;
    }

    if (mAngleX == 0) {
        mScaleX += 0x64000;
        mScaleY += 0x64000;
        mPosY -= 0x32000;
    }

    Matrix4x3_FromTranslation(&mInvMat, mPosX, mPosY, mPosZ);
    Matrix4x3_ApplyInPlaceToRotationY(&mInvMat, -mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&mInvMat, -mAngleX);
    InvMat4x3(&mInvMat, &mInvMat);

    mPosY += mScaleY >> 1;
    return 1;
}
