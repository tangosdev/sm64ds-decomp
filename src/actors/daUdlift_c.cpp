//cpp
/* ov095 daUdlift_c: the up/down lift shared by Big Boo's Haunt
 * (UDLIFT_TERESA), Hazy Maze Cave (UDLIFT) and Rainbow Ride (RC_RIFT02).
 * .text 0x02135ff4..0x02136764, 11 functions.
 *
 * A dBgActor_c that waits at one end of its shaft, travels when a player
 * steps on, and stops with a thud at the other end. Behavior runs one of five
 * states out of a pointer-to-member table that the module's static
 * initializer fills:
 *
 *   0 StateWait          wait at the current end
 *   1 StateMoveUp        climb to mTopY
 *   2 StateMoveDown      descend to mBottomY
 *   3 StateStop          stopped; wait again once re-armed
 *   4 StateStopAtBottom  stopped at the bottom; climb once re-armed
 *
 * daUdlift_c is the cartridge's RTTI spelling: _ZTS at ov095 0x021375bc is
 * "10daUdlift_c", and the _ZTI at 0x021375b0 names the vtable at 0x02137628.
 * The state names are reconstructed; the ROM records no member names.
 *
 * The destructor is inline and empty in the class header, so InitResources
 * is the key function and this TU emits the vtable and RTTI. D1 and D0 come
 * from the header.
 *
 * mwccarm 2004/b56 emits .text in the reverse of source order, so the
 * functions are written from the highest ROM address down. Do not reorder.
 *
 * dBgW_KcMbg::SetFile, dBgActor_c::IsClsnInRange and dActor_c::Earthquake
 * are called by their mangled names: each takes Fix12<int> by value, and the
 * member call with a real Fix12<int> changes the code (wall 6az).
 */

#include "daUdlift_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Sound.h"

typedef void (daUdlift_c::*State)();

void ApproachLinear(int &value, int target, int step);

extern "C" {
/* Per-variant model and collision files, and the CLPS block handed to
 * dBgW_KcMbg::SetFile, all indexed by mVariant. */
extern SharedFilePtr *data_ov095_02136f68[];
extern SharedFilePtr *data_ov095_02136f74[];
extern CLPS_Block *data_ov095_021375a4[];
/* The state table, indexed by mState. */
extern State data_ov095_02137910[];

void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale,
    s16 angleY, CLPS_Block *clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, Vector3 *pos, int strength);
void func_020393d4(void *p, void *v);
void func_020393c4(void *p, void *v);
/* The rider-collision callback; it and the factories are their own sources. */
void func_ov095_02136788(void *a, void *b, void *c);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daUdlift_c13InitResourcesEv
int daUdlift_c::InitResources()
{
    mMode = 0;
    /* The (int) flags keep the branch shape the ROM has. */
    if ((int)(actorID == 0x20) != 0) {
        mVariant = 0;
    } else if ((int)(actorID == 0x21) != 0) {
        mVariant = 1;
    } else if ((int)(actorID == 0x83) != 0) {
        mVariant = 2;
        mMode = 1;
    } else {
        return 0;
    }

    mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov095_02136f68[mVariant]), 1, -1);
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)dBgW_Kc::LoadFile(*data_ov095_02136f74[mVariant]),
        &mClsnMat, 0x199, mAngleY, data_ov095_021375a4[mVariant]);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    func_020393c4(&mMeshCollider, (void *)&func_ov095_02136788);
    mRider = 0;
    mState = 0;

    /* The top of the shaft is where the lift was placed and the spawn word
       says how far down it travels. The mMode == 2 arm is unreachable here:
       only 0 and 1 are ever stored. */
    if (mMode == 2)
        mTopY = mPosY + ((u16)mPrevAngleZ << 12);
    else
        mTopY = mPosY;
    mBottomY = mTopY - ((u16)mPrevAngleX << 12);
    mMiddleY = (mTopY + mBottomY) / 2;
    mIsAtBottom = 0;
    mIsArmed = 1;
    mIsRidden = 0;
    mSoundHandle = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daUdlift_c8BehaviorEv
int daUdlift_c::Behavior()
{
    int oldState;

    mClosestPlayer = ClosestPlayer();
    oldState = mState;
    (this->*data_ov095_02137910[oldState])();
    mStateTimer += 1;
    if (oldState != mState)
        mStateTimer = 0;

    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();

    /* Waiting or stopped: once the rider has landed somewhere else, forget
       them and arm the trigger again. */
    if (mState == 0 || mState == 3 || mState == 4) {
        if (mRider != 0 && mRider->IsInAir() == 0 && mIsRidden == 0) {
            mRider = 0;
            mIsArmed = 1;
        }
    }
    mIsRidden = 0;
    if (mClosestPlayer != 0)
        mPlayerPosY = mClosestPlayer->mPosY;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daUdlift_c6RenderEv
int daUdlift_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daUdlift_c16CleanupResourcesEv
int daUdlift_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov095_02136f68[mVariant]->Release();
    data_ov095_02136f74[mVariant]->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daUdlift_c9StateWaitEv
/* State 0, waiting. A newly boarded rider sends the lift to the other end.
 * Otherwise, when the closest player crosses mMiddleY away from the lift, the
 * lift follows: it jumps straight there if the player was already past the
 * middle last frame, and travels there if not. */
void daUdlift_c::StateWait()
{
    Player *player;
    int middleY;

    if (mIsAtBottom == 0) {
        if (mRider != 0 && mIsArmed == 1) {
            mState = 2;
            mIsArmed = 0;
            return;
        }
        player = mClosestPlayer;
        if (player == 0)
            return;
        middleY = mMiddleY;
        if (player->mPosY > middleY)
            return;
        /* The (int) flag keeps the branch shape the ROM has. */
        if ((int)(actorID == 0x83) != 0)
            return;
        if (mPlayerPosY > middleY) {
            mPosY = mBottomY;
            mIsAtBottom = 1;
        } else {
            mState = 2;
        }
        return;
    }

    if (mRider != 0 && mIsArmed == 1) {
        mState = 1;
        mIsArmed = 0;
        return;
    }
    player = mClosestPlayer;
    if (player == 0)
        return;
    middleY = mMiddleY;
    if (player->mPosY < middleY)
        return;
    if (mPlayerPosY < middleY) {
        mPosY = mTopY;
        mIsAtBottom = 0;
    } else {
        mState = 1;
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daUdlift_c11StateMoveUpEv
/* State 1, climbing: speed up toward 10 units a frame and stop at mTopY. */
void daUdlift_c::StateMoveUp()
{
    mSoundHandle = Sound::PlayLong(
        mSoundHandle, 3, 0x82, *(Vector3 *)&mCamSpacePosX, 0);
    ApproachLinear(mVertSpeed, 0xa000, 0x2000);
    mPosY += mVertSpeed;
    if (mPosY < mTopY)
        return;
    mPosY = mTopY;
    mIsAtBottom = 0;

    if (mMode != 0) {
        mState = 3;
        return;
    }
    if (mClosestPlayer != 0 && mClosestPlayer->mPosY <= mMiddleY) {
        mState = 2;
        return;
    }
    mState = 3;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daUdlift_c13StateMoveDownEv
/* State 2, descending: speed up toward 10 units a frame and stop at
 * mBottomY. */
void daUdlift_c::StateMoveDown()
{
    mSoundHandle = Sound::PlayLong(
        mSoundHandle, 3, 0x82, *(Vector3 *)&mCamSpacePosX, 0);
    /* The (int) flag keeps the branch shape the ROM has. */
    if (mStateTimer == 0 && (int)(actorID == 0x21) != 0)
        Sound::PlayBank3(0x40, *(Vector3 *)&mCamSpacePosX);

    ApproachLinear(mVertSpeed, -0xa000, -0x2000);
    mPosY += mVertSpeed;
    if (mPosY > mBottomY)
        return;
    mPosY = mBottomY;
    mIsAtBottom = 1;

    if (mMode == 1) {
        mState = 4;
        return;
    }
    if (mMode == 2) {
        mState = 3;
        return;
    }
    if (mClosestPlayer != 0 && mClosestPlayer->mPosY >= mMiddleY) {
        mState = 1;
        return;
    }
    mState = 3;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daUdlift_c17StateStopAtBottomEv
/* State 4, stopped at the bottom: land with a thud and a camera shake, and
 * climb again once re-armed. */
void daUdlift_c::StateStopAtBottom()
{
    mVertSpeed = 0;
    if (mStateTimer == 0) {
        Sound::PlayBank3(0x6b, *(Vector3 *)&mCamSpacePosX);
        Vector3 pos;
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &pos, 0x320000);
    }
    if (mIsArmed == 1)
        mState = 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daUdlift_c9StateStopEv
/* State 3, stopped: the same landing, then wait again once re-armed. */
void daUdlift_c::StateStop()
{
    mVertSpeed = 0;
    if (mStateTimer == 0) {
        Sound::PlayBank3(0x6b, *(Vector3 *)&mCamSpacePosX);
        Vector3 pos;
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &pos, 0x320000);
    }
    if (mIsArmed == 1)
        mState = 0;
}
