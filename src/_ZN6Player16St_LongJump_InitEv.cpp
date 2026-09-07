//cpp
// @symbol _ZN6Player16St_LongJump_InitEv
/* recovered: named members + shared header, real C++ method
 *
 * Three guards get first refusal on the long jump; if any takes it, this
 * returns without entering. Otherwise: airborne flags, the jump animation,
 * an upward kick, then horizontal speed scaled by 1.5 (0x1800 in 12.4 fixed
 * point, with the usual +0x800 round-to-nearest) and clamped to 0x3c000.
 */
#include "Player.h"
extern "C" {
extern int func_ov002_020e2be4(void* self);
extern int func_ov002_020e2ba8(void* c);
extern int func_ov002_020e2b6c(void* c);
extern void func_ov002_020e2ad0(void* c);
/* SetAnim takes a Fix12<int> by value -- the mwccarm 6az wall, runbook
   section 7 -- so it stays extern "C" with a scalar in that slot. */
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* self, unsigned int a, int b, int c, unsigned int d);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, Vector3 const & v);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, Vector3 const & v);
}

int Player::St_LongJump_Init()
{
    mJumpedFromQuicksand = 0;
    if (func_ov002_020e2be4(this)) {
        return 1;
    }
    if (func_ov002_020e2ba8(this)) {
        return 1;
    }
    if (func_ov002_020e2b6c(this)) {
        return 1;
    }
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x1a, 0x40000000, 0x1000, 0);
    mVertSpeed = 0x1e000;
    func_ov002_020e2ad0(this);
    mVertAccel = -0x2000;
    mHorzSpeed = (int)(((s64)mHorzSpeed * 0x1800 + 0x800) >> 12);
    if (mHorzSpeed >= 0x3c000) {
        mHorzSpeed = 0x3c000;
    }
    /* mCamSpacePosX/Y/Z are three consecutive words; dActor_c.h declares them
       individually rather than as one Vector3. */
    if (mIsMetal == 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3((u32)mGroundSoundType + 0x30, *(const Vector3*)&mCamSpacePosX);
    } else {
        _ZN5Sound9PlayBank0EjRK7Vector3(0xa0, *(const Vector3*)&mCamSpacePosX);
    }
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 4, *(const Vector3*)&mCamSpacePosX);
    return 1;
}
