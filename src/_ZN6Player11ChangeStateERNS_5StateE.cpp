//cpp
// @symbol _ZN6Player11ChangeStateERNS_5StateE
/* recovered: named members + shared header, real C++ method,
 * declarations from a shared header
 *
 * The parameter is a REFERENCE -- the ROM name says RNS_5StateE -- so it is
 * taken as one and bound to a local pointer, leaving every expression in the
 * body exactly as it was.
 */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Player.h"
extern "C" {
extern void func_ov002_020d4540(void *p);
extern void func_ov002_020c9e18(void *c);
extern void func_0200d81c(void *thiz, int playerID);

extern void *data_0209f318;
extern Player::State data_ov002_0211022c;
extern Player::State data_ov002_0211013c;
extern Player::State data_ov002_0211067c;
extern Player::State data_ov002_021106ac;
extern Player::State data_ov002_02110364;
}

int Player::ChangeState(State &state) {
    State *newState = &state;
    mRequestedState = newState;

    {
        State *cur;
        int ok;
        /* The null-checks below stay as raw offset arithmetic on purpose.
           Spelling either as `*(void**)&cur->mCleanup == 0` costs 4 words:
           taking the ADDRESS of a pointer-to-member makes mwcc materialise
           the whole 8-byte pmf first. Reading one to CALL it is free -- the
           calls below use the named members. Measured both ways. */
        cur = mState;
        if (cur == 0) { ok = 1; goto check_ok; }
        if (*(int*)((char*)cur+0x10) == 0) { ok = 1; goto check_ok; }
        ok = (this->*(cur->mCleanup))();
    check_ok:
        if (!ok) return 0;
    }

    if (mState == &data_ov002_0211022c
        && (unsigned short)(mStateFlags & 0x400) == 0
        && mIsNoControl != 0
        && newState != &data_ov002_0211013c
        && newState != &data_ov002_0211067c
        && newState != &data_ov002_021106ac
        && newState != &data_ov002_0211022c)
    {
        return 0;
    }

    {
        int b = (mHeldObj != 0);
        if (b) {
        if (newState == &data_ov002_0211013c) {
            mStateStep = 1;
            newState = &data_ov002_02110364;
        }
        }
    }

    mLoopingSoundHandle = 0;
    unk_630 = 0;
    mParticle2 = unk_630;
    unk_628 = mParticle2;

    mPrevState = mState;
    mState = newState;

    unk_717 = 0;
    mIsBodyClsnEnabled = 1;
    unk_716 = 0;

    unk_694 = 0;
    unk_690 = unk_694;

    mIsInAirState = 0;
    unk_654 = 0;

    mAngleZ = 0;
    mAngleX = mAngleZ;

    mIsControlDisabled = 0;
    unk_6ec = 0;
    unk_726 = 0;

    mStateFlags &= ~0x200;

    {
        int v1 = 0x4000;
        int v2 = 0x4b000;
        v1 = -v1;
        v2 = -v2;
        mVertAccel = v1;
        mTerminalVelocity = v2;
    }

    mStateTimer = 0;

    func_ov002_020d4540(this);

    mIsTakingDamage = 0;
    func_ov002_020c9e18(this);

    func_ov002_020e6780((char *)this);

    if (data_0209f318 != 0) {
        func_0200d81c(data_0209f318, mPlayerNo);
    }

    {
        State *s = mState;
        if (*(int*)s == 0) return 1;
        return (this->*(s->mInit))();
    }
}
