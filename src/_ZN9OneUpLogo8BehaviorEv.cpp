//cpp
// @symbol _ZN9OneUpLogo8BehaviorEv

#include "OneUpLogo.h"

extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, s32 x, s32 y, s32 z);
void Matrix4x3_FromTranslation(Matrix4x3 *matrix, s32 x, s32 y, s32 z);
}

int OneUpLogo::Behavior()
{
    Vector3 pos;

    if (mDelay != 0)
        return 1;

    mVertSpeed += mVertAccel;
    if (mVertSpeed < mTerminalVelocity)
        mVertSpeed = mTerminalVelocity;
    mPosY += mVertSpeed;

    switch (mState) {
    case 0:
        if (mPosY < mInitialPos.y) {
            mPosY = mInitialPos.y;
            mVertSpeed = 0xf000;
            mState++;
        }
        break;
    case 1:
        if (mPosY < mInitialPos.y) {
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0xd2, mPosX, *(volatile s32 *)&mPosY, mPosZ);
            MarkForDestruction();
        }
        break;
    }

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    if (mFollowActorID != 0) {
        dActor_c *other = FindWithID(mFollowActorID);
        if (other != 0) {
            Vector3 *otherPos = (Vector3 *)&other->mPosX;
            s32 otherY;
            pos.x = otherPos->x;
            pos.y = otherY = otherPos->y;
            pos.z = otherPos->z;
            pos.y = otherY + (mFollowYOffset + (mPosY - mInitialPos.y));
        }
    }
    Matrix4x3_FromTranslation(&mModel.mat4x3, pos.x >> 3, pos.y >> 3,
                              (*(volatile s32 *)&pos.z) >> 3);
    return 1;
}
