//cpp
#include "SnowmanHead.h"
#include "Player.h"

void ApproachLinear(short &value, short target, short step);
extern "C" int Vec3_HorzDist(const void *a, const void *b);
struct SnowmanHeadVec3 {
    s32 x;
    s32 y;
    s32 z;
};

int SnowmanHead::State3()
{
    SnowmanHeadVec3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x1c2000;

    switch (mSubstate) {
    case 0:
        mTalkPlayer = ClosestPlayer();
        if (Vec3_HorzDist((char *)this + 0x5c,
                          (char *)mTalkPlayer + 0x5c) < 0x118000) {
            if (mTalkPlayer->StartTalk(*this, true))
                mSubstate++;
        }
        break;
    case 1:
        if (mTalkPlayer->ShowMessage(
                *this, 0xb1, (Vector3 *)&pos, 0, 2))
            mSubstate++;
        break;
    case 2:
        if (mTalkPlayer->GetTalkState() == -1) {
            unsigned int param = (*(int *)&param1) & 0xf;
            param = param & 0xff;
            param |= 0x40;
            Spawn(0xb2, param, *(Vector3 *)((char *)this + 0x5c),
                  0, mAreaId, -1);
            mSubstate++;
        }
        break;
    }
    mTextureSequence.Advance();
    ApproachLinear(mAngleY, -0x4000, 0x514);
    mPosY = (int)0xffc427c0;
    return 1;
}
