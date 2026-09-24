//cpp
/* Production translation unit for ov095/daUdlift_c, hand-curated.
 * 11 function(s), .text 0x02135ff4..0x02136764.
 *
 * The up/down lift shared by Big Boo's Haunt (UDLIFT_TERESA), Hazy Maze Cave
 * (UDLIFT) and Rainbow Ride (RC_RIFT02): a dBgActor_c that waits at one end
 * of its shaft, travels when a player steps on, and stops with a thud at the
 * other end. Behavior runs one of five states out of a pointer-to-member
 * table that the module's static initializer fills:
 *
 *   0 func_ov095_02136368  wait at the current end
 *   1 func_ov095_02136298  climb to mTopY
 *   2 func_ov095_02136178  descend to mBottomY
 *   3 func_ov095_02136090  stopped; wait again once re-armed
 *   4 func_ov095_02136104  stopped at the bottom; climb once re-armed
 *
 * The state bodies are written in state order below, which is also the
 * reverse of their ROM order, as it should be.
 *
 * NAME: daUdlift_c is the cartridge's RTTI spelling. _ZTS at ov095
 * 0x021375bc is the string "10daUdlift_c", and the _ZTI at 0x021375b0 names
 * the vtable at 0x02137628 as this class's. The tree called the class
 * UpDownLiftBbh until then.
 *
 * THE DESTRUCTOR IS INLINE AND EMPTY in the class header, so InitResources is
 * the key function and this TU emits _ZTV10daUdlift_c, _ZTI10daUdlift_c and
 * _ZTS10daUdlift_c with the inherited bases' RTTI records. D1 and D0 are
 * emitted from the header, so there is no destructor text here to mark.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * Leftover: dBgW_KcMbg::SetFile, dBgActor_c::IsClsnInRange and
 *   dActor_c::Earthquake stay mangled. Each takes Fix12<int> by value (wall
 *   6az), and a member call homes the argument and changes the code.
 * Leftover: the five state bodies are called through the table as members of
 *   this class, but they keep their func_ov095_* linker names as C-linkage
 *   helpers over a daUdlift_c pointer. A member spelling would coin five
 *   mangled names; that renaming is separate work.
 * Leftover: the state table (data_ov095_02137910, .bss), the model and
 *   collision file tables and the CLPS table are unnamed ov095 rows this TU
 *   does not own.
 * Leftover: the collision callback func_ov095_02136788 and its body
 *   func_ov095_02136764, and the three factories, sit past this run's right
 *   edge and stay one-function sources; tu_map.py ends this candidate at
 *   0x02136764.
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
void func_ov095_02136788(void *a, void *b, void *c);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daUdlift_c13InitResourcesEv
int daUdlift_c::InitResources()
{
  int idx;
  void *new_var;
  u16 t;
  int b;
  unk_349 = 0;
  t = actorID;
  b = (int) (t == 0x20);
  if (b != 0)
  {
    mVariant = 0;
    goto load;
  }
  b = (int) (t == 0x21);
  if (b != 0)
  {
    mVariant = 1;
    goto load;
  }
  b = (int) (t == 0x83);
  if (b != 0)
  {
    mVariant = 2;
    unk_349 = 1;
    goto load;
  }
  return 0;
  load:
  idx = mVariant;

  mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov095_02136f68[idx]), 1, -1);
  new_var = (void *)&dBgW::UpdatePosWithTransform;
  UpdateClsnPosAndRot();
  idx = mVariant;
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
      &mMeshCollider, (KCL_File *)dBgW_Kc::LoadFile(*data_ov095_02136f74[idx]),
      &mClsnMat, 0x199, mAngleY, data_ov095_021375a4[idx]);
  func_020393d4(&mMeshCollider, new_var);
  func_020393c4(&mMeshCollider, (void *)&func_ov095_02136788);
  mRider = 0;
  mState = 0;
  /* Top of the shaft is where it was placed; the spawn word says how far down
     it travels, and it stops halfway on the way back up. */
  if (unk_349 == 2)
  {
    mTopY = mPosY + ((u16)mPrevAngleZ << 12);
  }
  else
  {
    mTopY = mPosY;
  }
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
    int old;
    mClosestPlayer = ClosestPlayer();
    old = mState;
    (this->*data_ov095_02137910[old])();
    mStateTimer += 1;
    if (old != mState)
        mStateTimer = 0;

    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();

    /* Waiting or stopped: once the rider has landed somewhere else, forget
       them and arm the trigger again. */
    if (mState == 0 || (unsigned)(mState - 3) <= 1) {
        Player *player = mRider;
        if (player != 0) {
            if (player->IsInAir() == 0) {
                if (mIsRidden == 0) {
                    mRider = 0;
                    mIsArmed = 1;
                }
            }
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
// @symbol func_ov095_02136368
/* State 0, waiting. A newly boarded rider sends the lift to the other end.
 * Otherwise, when the closest player crosses mMiddleY away from the lift, the
 * lift follows: it jumps straight there if the player was already past the
 * middle last frame, and travels there if not. */
extern "C" void func_ov095_02136368(daUdlift_c *lift)
{
    if (lift->mIsAtBottom == 0) {
        if (lift->mRider != 0) {
            if (lift->mIsArmed == 1) {
                lift->mState = 2;
                lift->mIsArmed = 0;
                return;
            }
        }
        {
            Player *p = lift->mClosestPlayer;
            int lim;
            if (p == 0) return;
            lim = lift->mMiddleY;
            if (p->mPosY > lim) return;
            /* Tested as a flag first: tested inline, the branch comes out
               a different shape. */
            {
                int b = (int)(lift->actorID == 0x83);
                if (b != 0) return;
            }
            if (lift->mPlayerPosY > lim) {
                lift->mPosY = lift->mBottomY;
                lift->mIsAtBottom = 1;
            } else {
                lift->mState = 2;
            }
        }
        return;
    }
    if (lift->mRider != 0) {
        if (lift->mIsArmed == 1) {
            lift->mState = 1;
            lift->mIsArmed = 0;
            return;
        }
    }
    {
        Player *p = lift->mClosestPlayer;
        int lim;
        if (p == 0) return;
        lim = lift->mMiddleY;
        if (p->mPosY < lim) return;
        if (lift->mPlayerPosY < lim) {
            lift->mPosY = lift->mTopY;
            lift->mIsAtBottom = 0;
        } else {
            lift->mState = 1;
        }
    }
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov095_02136298
/* State 1, climbing: speed up toward 10 units a frame and stop at mTopY. */
extern "C" void func_ov095_02136298(daUdlift_c *lift)
{
    lift->mSoundHandle = Sound::PlayLong(
        lift->mSoundHandle, 3, 0x82, *(Vector3 *)&lift->mCamSpacePosX, 0);
    ApproachLinear(lift->mVertSpeed, 0xa000, 0x2000);
    lift->mPosY += lift->mVertSpeed;
    if (lift->mPosY < lift->mTopY) return;
    lift->mPosY = lift->mTopY;
    lift->mIsAtBottom = 0;
    if (lift->unk_349 != 0) {
        lift->mState = 3;
        return;
    }
    Player *p = lift->mClosestPlayer;
    if (p != 0) {
        if (p->mPosY <= lift->mMiddleY) {
            lift->mState = 2;
            return;
        }
    }
    lift->mState = 3;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov095_02136178
/* State 2, descending: speed up toward 10 units a frame and stop at
 * mBottomY. */
extern "C" void func_ov095_02136178(daUdlift_c *lift)
{
    lift->mSoundHandle = Sound::PlayLong(
        lift->mSoundHandle, 3, 0x82, *(Vector3 *)&lift->mCamSpacePosX, 0);
    if (lift->mStateTimer == 0) {
        /* A flag first, for the same reason as in state 0. */
        int b = (int)(lift->actorID == 0x21);
        if (b != 0)
            Sound::PlayBank3(0x40, *(Vector3 *)&lift->mCamSpacePosX);
    }

    ApproachLinear(lift->mVertSpeed, -0xa000, -0x2000);
    lift->mPosY += lift->mVertSpeed;

    if (lift->mPosY > lift->mBottomY) return;
    lift->mPosY = lift->mBottomY;
    lift->mIsAtBottom = 1;

    {
        int s = lift->unk_349;
        if (s == 1) { lift->mState = 4; return; }
        if (s == 2) { lift->mState = 3; return; }
    }
    if (lift->mClosestPlayer != 0) {
        if (lift->mClosestPlayer->mPosY >= lift->mMiddleY) {
            lift->mState = 1;
            return;
        }
    }
    lift->mState = 3;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov095_02136104
/* State 4, stopped at the bottom: land with a thud and a camera shake, and
 * climb again once re-armed. */
extern "C" void func_ov095_02136104(daUdlift_c *lift)
{
    lift->mVertSpeed = 0;
    if (lift->mStateTimer == 0) {
        Sound::PlayBank3(0x6b, *(Vector3 *)&lift->mCamSpacePosX);
        Vector3 pos;
        pos.x = lift->mPosX;
        pos.y = lift->mPosY;
        pos.z = lift->mPosZ;
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(lift, &pos, 0x320000);
    }
    if (lift->mIsArmed == 1)
        lift->mState = 1;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov095_02136090
/* State 3, stopped: the same landing, then wait again once re-armed. */
extern "C" void func_ov095_02136090(daUdlift_c *lift)
{
    lift->mVertSpeed = 0;
    if (lift->mStateTimer == 0) {
        Sound::PlayBank3(0x6b, *(Vector3 *)&lift->mCamSpacePosX);
        Vector3 pos;
        pos.x = lift->mPosX;
        pos.y = lift->mPosY;
        pos.z = lift->mPosZ;
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(lift, &pos, 0x320000);
    }
    if (lift->mIsArmed == 1)
        lift->mState = 0;
}
