//cpp
/* ov095 daUdlift_c -- the up/down lift in Big Boo's Haunt (UDLIFT_TERESA),
 * Hazy Maze Cave (UDLIFT) and Rainbow Ride (RC_RIFT02).
 *
 * It waits at one end of a vertical shaft, travels when a player steps on,
 * and stops with a thud at the other end. Behavior dispatches mState through
 * data_ov095_02137910, the pointer-to-member table the overlay static
 * initializer fills:
 *
 *   0 StateWait          wait at the current end
 *   1 StateMoveUp        climb to mTopY
 *   2 StateMoveDown      descend to mBottomY
 *   3 StateStop          stopped; wait again once re-armed
 *   4 StateStopAtBottom  stopped at the bottom; climb once re-armed
 *
 * mMode is 0 on the Haunt and Cave lifts (follow the player) and 1 on
 * Rainbow Ride (park in StateStopAtBottom). InitResources never stores 2;
 * the mMode == 2 arm is still in the cartridge.
 *
 * The destructor is inline and empty in include/daUdlift_c.h, so
 * InitResources is the key function and the header emits D1 then D0 with
 * no D2. mwccarm emits .text in reverse source order. The functions below
 * are written from the highest address down. Do not reorder them, and do
 * not write the destructor out of line.
 *
 * deslop leftovers:
 * - InitResources, StateWait, StateMoveDown: the actorID test has to be
 *   `(int)(actorID == id) != 0`. A plain `actorID == id`, and that compare
 *   without the `(int)` cast, both shrink the same three functions.
 *   InitResources 0x18c->0x160, and its LoadFile reloc then names
 *   _ZN7dBgW_Kc8LoadFileER13SharedFilePtr instead of 0x02017a3c.
 *   StateWait 0xf4->0xe8. StateMoveDown 0x120->0x114.
 * - InitResources: mMeshCollider.SetFile with a Fix12<int>{0x199} scale
 *   grows the function 0x18c->0x190, mis-aims three relocs (first
 *   data_ov095_02136f68 vs data_ov095_02136f74) and emits unlicensed @546.
 *   Passing the scale as an int does not compile. The mangled int bridge
 *   stays. IsClsnInRange is the same bridge: dBgActor_c.h has no member.
 * - InitResources: func_020393d4 and func_020393c4 store dBgW+0x18
 *   (beforeClsnCallback) and dBgW+0x1c (the rider callback). Writing those
 *   fields directly shrinks InitResources 0x18c->0x184 and drops the two
 *   call relocs.
 * - StateStop, StateStopAtBottom: Earthquake copies mPos into a stack
 *   Vector3. Passing (Vector3 *)&mPosX shrinks each function 0x74->0x54
 *   and stops emitting _ZN7Vector3D1Ev, which this TU deadstrips. A shared
 *   Land() is not inlined. It is emitted as _Z4LandR10daUdlift_ci and both
 *   states shrink to 0x10.
 * - StateMoveDown: one Vector3& over mCamSpacePosX, shared by PlayLong and
 *   PlayBank3, grows the function 0x120->0x124. The two puns stay. A
 *   CamPos() helper is emitted as _Z6CamPosR10daUdlift_c and grows
 *   StateMoveUp 0xd0->0xd4, StateMoveDown 0x120->0x12c, and both stops
 *   0x74->0x78.
 * - data_ov095_02136f68, data_ov095_02136f74, data_ov095_021375a4 and
 *   data_ov095_02137910 keep the address names. The profiles are not in
 *   this TU.
 */

#include "daUdlift_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Sound.h"

typedef void (daUdlift_c::*State)();

/* Profile ids from the ROM debug-name table. */
enum {
    UDLIFT_TERESA = 0x20, /* Big Boo's Haunt */
    UDLIFT = 0x21,        /* Hazy Maze Cave */
    RC_RIFT02 = 0x83      /* Rainbow Ride */
};

void ApproachLinear(int &value, int target, int step);

extern "C" {
/* Per-variant model and collision files, and the CLPS block handed to
 * dBgW_KcMbg::SetFile, all indexed by mVariant. */
extern SharedFilePtr *data_ov095_02136f68[];
extern SharedFilePtr *data_ov095_02136f74[];
extern CLPS_Block *data_ov095_021375a4[];
/* The state table, indexed by mState. */
extern State data_ov095_02137910[];

/* Fix12<int> by value. The member call grows InitResources; see the leftovers. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale,
    s16 angleY, CLPS_Block *clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, Vector3 *pos, int strength);
/* dBgW+0x18 and dBgW+0x1c. A direct store shrinks InitResources. */
void func_020393d4(void *collider, void *callback);
void func_020393c4(void *collider, void *callback);
/* The rider-collision callback. It and the factories are their own sources. */
void func_ov095_02136788(void *a, void *b, void *c);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daUdlift_c13InitResourcesEv
int daUdlift_c::InitResources()
{
    mMode = 0;
    /* The (int) flags keep the branch shape. A plain compare shrinks this
       function and retargets LoadFile. */
    if ((int)(actorID == UDLIFT_TERESA) != 0) {
        mVariant = 0;
    } else if ((int)(actorID == UDLIFT) != 0) {
        mVariant = 1;
    } else if ((int)(actorID == RC_RIFT02) != 0) {
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
        /* Rainbow Ride does not chase across the middle. The (int) flag
           keeps the branch; a plain compare shrinks this function. */
        if ((int)(actorID == RC_RIFT02) != 0)
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
    Vector3 &camPos = *(Vector3 *)&mCamSpacePosX;
    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x82, camPos, 0);
    ApproachLinear(mVertSpeed, (10 << 12), (2 << 12));
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
    /* HMC plays a one-shot on the first frame. The (int) flag keeps the
       branch; a plain compare shrinks this function. One Vector3& shared
       with PlayLong also grows it, so the second pun stays. */
    if (mStateTimer == 0 && (int)(actorID == UDLIFT) != 0)
        Sound::PlayBank3(0x40, *(Vector3 *)&mCamSpacePosX);

    ApproachLinear(mVertSpeed, -(10 << 12), -(2 << 12));
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
        Vector3 &camPos = *(Vector3 *)&mCamSpacePosX;
        Sound::PlayBank3(0x6b, camPos);
        Vector3 pos = {mPosX, mPosY, mPosZ};
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &pos, (800 << 12));
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
        Vector3 &camPos = *(Vector3 *)&mCamSpacePosX;
        Sound::PlayBank3(0x6b, camPos);
        Vector3 pos = {mPosX, mPosY, mPosZ};
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &pos, (800 << 12));
    }
    if (mIsArmed == 1)
        mState = 0;
}
