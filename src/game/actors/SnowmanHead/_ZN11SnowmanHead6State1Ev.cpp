//cpp
#include "SnowmanHead.h"
#include "Player.h"

int ApproachLinear(short &value, short target, short step);
extern "C" short Vec3_HorzAngle(const void *a, const void *b);
struct SnowmanHeadVec3 {
    s32 x;
    s32 y;
    s32 z;
};

int SnowmanHead::State1()
{
    switch (mSubstate) {
    case 0:
    {
        short angle = Vec3_HorzAngle((char *)this + 0x5c,
                                    (char *)mTalkPlayer + 0x5c);
        if (ApproachLinear(mAngleY, angle, 0x514)) {
            SnowmanHeadVec3 pos;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            pos.y += 0xfa000;
            if (mTalkPlayer->ShowMessage(
                    *this, 0xaf, (Vector3 *)&pos, 0, 0))
                mSubstate++;
        }
        break;
    }
    case 1:
        if (mTalkPlayer->GetTalkState() == -1)
            SetState(0);
        break;
    }
    return 1;
}
