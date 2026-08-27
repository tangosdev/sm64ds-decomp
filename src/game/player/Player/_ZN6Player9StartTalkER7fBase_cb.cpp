//cpp
// @symbol _ZN6Player9StartTalkER7fBase_cb
/* recovered: named members + shared header, real C++ method
 *
 * Entering conversation. Already talking to this actor with mStateStep 0 is
 * success; talking to anything else is refusal. Otherwise only a short list of
 * states may start a conversation, and when b_ is false the player must also
 * be pressing a button and be walking slowly, crouching or waiting.
 *
 * The third parameter is bool -- the ROM name ends in `b`, and a real method
 * is mangled FROM the declared types.
 */
#include "Player.h"
extern "C" {

extern Player::State data_ov002_0211046c;
extern Player::State data_ov002_0211013c;
extern Player::State data_ov002_02110154;
extern Player::State data_ov002_021104e4;
extern Player::State data_ov002_02110514;
extern Player::State data_ov002_02110364;
extern Player::State data_ov002_02110424;
extern Player::State data_ov002_021105a4;
extern Player::State data_ov002_0211043c;
extern u8 data_020a0e40;
extern u8 data_0209f49e[];


int Player::StartTalk(fBase_c & actor_, bool b_)
{
    fBase_c *actor = &actor_;
    if (IsState(data_ov002_0211046c)) {
        if (actor == *(fBase_c **)&mTalkActor && mStateStep == 0)
            return 1;
        return 0;
    }

    if (!(IsState(data_ov002_0211013c) ||
          IsState(data_ov002_02110154) ||
          IsState(data_ov002_021104e4) ||
          IsState(data_ov002_02110514) ||
          IsState(data_ov002_02110364) ||
          IsState(data_ov002_02110424) ||
          IsState(data_ov002_021105a4) ||
          IsState(data_ov002_0211043c)))
        return 0;

    if (b_ == 0) {
        if (!(*(u16 *)(data_0209f49e + (u8)data_020a0e40 * 0x18) & 3))
            goto ret0;

        if (!((IsState(data_ov002_0211013c) && mHorzSpeed < 0x8000) ||
              IsState(data_ov002_0211043c) ||
              IsState(data_ov002_02110154)))
            goto ret0;

        mStateStep = 0;
        ChangeState(data_ov002_0211046c);
        *(fBase_c **)&mTalkActor = actor;
        return 1;
    } else {
        if (IsState(data_ov002_02110424) ||
            IsState(data_ov002_021105a4)) {
            *(u16 *)(((long long)((char *)this + 0x6ce))) |= 0x40;
            return 0;
        }

        mStateStep = 0;
        ChangeState(data_ov002_0211046c);
        *(fBase_c **)&mTalkActor = actor;
        return 1;
    }
ret0:
    return 0;
}
}
