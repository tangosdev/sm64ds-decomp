//cpp
#include "SnowmanHead.h"
#include "Player.h"

void ApproachLinear(short &value, short target, short step);
extern "C" int Vec3_HorzDist(const void *a, const void *b);
extern "C" unsigned char DecIfAbove0_Byte(unsigned char *value);

int SnowmanHead::State0()
{
    ApproachLinear(mAngleY, -0x4000, 0x514);
    mTextureSequence.Advance();
    if (unk_336) {
        SetState(2);
        return 1;
    }
    if (DecIfAbove0_Byte(&mStateTimer) == 0) {
        Player *player = ClosestPlayer();
        if (Vec3_HorzDist((char *)this + 0x5c, (char *)player + 0x5c) < 0x118000
            && player->StartTalk(*this, true) != 0) {
            mTalkPlayer = player;
            SetState(1);
        }
    }
    return 1;
}
