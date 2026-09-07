//cpp
#include "types.h"
// @symbol _ZN9LightBeam8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "LightBeam.h"
#include "Message.h"
#include "Player.h"
extern "C" {
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(const Vector3* v, const Matrix4x3* m, Vector3* res);
extern void Vec3_Add(Vector3* out, const Vector3* a, const Vector3* b);
extern int _Z14ApproachLinearRsss(s16* val, s16 target, s16 step);
extern s16 Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
}

extern Matrix4x3 data_020a0e68;

int LightBeam::Behavior()
{
    Vector3 vIn, vMid, vRes;
    Vector3 hv;
    Vector3 vIn2, vMid2, vRes2;

    if (mTalkPlayer != 0) {
        Player *p;
        if (!_ZN5Sound20PlaySmallSecretSoundEP8dActor_cPt(this, mSoundTimers))
            return 1;
        p = (Player *)mTalkPlayer;

        vIn.x = -0x3b0000;
        vIn.y = 0x200000;
        vIn.z = 0x120000;
        vMid.x = 0;
        vMid.y = 0;
        vMid.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MulVec3Mat4x3(&vIn, &data_020a0e68, &vMid);
        Vec3_Add(&vRes, (Vector3 *)&mPosX, &vMid);

        switch (p->GetTalkState()) {
        case 0:
            if (_Z14ApproachLinearRsss(&p->mAngleY, mAngleY + 0x8000, 0x800)) {
                Message::PrepareTalk();
                p->ShowMessage(*this, 0x192, &vRes, 0, 0);
            }
            break;
        case 1:
            break;
        default:
            Message::EndTalk();
            mTalkPlayer = 0;
            break;
        }
    } else {
        if (mCylinder.hitFlags & 0x8000000) {
            dActor_c *o = dActor_c::FindWithID(mCylinder.otherOwner);
            if (o) {
                int b = (int)(o->actorID == 0xbf);
                if (b) {
                    Vector3* op = (Vector3 *)&o->mPosX;
                    hv = *op;
                    if (AngleDiff(Vec3_HorzAngle((Vector3 *)&mPosX, &hv), mAngleY) < 0x4000) {
                        if (*(s32*)((char*)o + 0x664) == 0xd) {
                            if (((Player *)o)->StartTalk(*this, false)) {
                                mTalkPlayer = o;
                                mSoundTimers[0] = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    vIn2.x = -0x3b0000;
    vIn2.y = 0;
    vIn2.z = 0x320000;
    vMid2.x = 0;
    vMid2.y = 0;
    vMid2.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MulVec3Mat4x3(&vIn2, &data_020a0e68, &vMid2);
    Vec3_Add(&vRes2, (Vector3 *)&mPosX, &vMid2);
    mCylinder.pos.x = vRes2.x;
    mCylinder.pos.y = vRes2.y;
    mCylinder.pos.z = vRes2.z;
    mCylinder.radius = 0xf0000;
    mCylinder.Clear();
    mCylinder.Update();
    return 1;
}
