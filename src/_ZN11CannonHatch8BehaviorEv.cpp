//cpp
#include "CannonHatch.h"

extern "C" {
char *_ZN8dActor_c13ClosestPlayerEv(char *self);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
void Matrix4x3_FromRotationY(void *m, int angle);
void MulVec3Mat4x3(Vector3 *v, void *m, Vector3 *dst);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *d);
int Vec3_Dist(void *a, void *b);
int Vec3_HorzDist(void *a, void *b);
CannonHatch *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, CannonHatch *after);
extern Matrix4x3 data_020a0e68;
}

int CannonHatch::Behavior()
{
    if (mCannonOpen != 0) {
        if (*(u8 *)(_ZN8dActor_c13ClosestPlayerEv((char *)this) + 0x703) != 0) {
            mPosX = mHomePosX;
            mPosY = mHomePosY;
            mPosZ = mHomePosZ;
            if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0)) {
                _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
                _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
            }
        } else {
            if (mMeshCollider.IsEnabled()) {
                mMeshCollider.Disable();
            }
        }
        return 1;
    }
    if (mOpening != 0) {
        Vector3 in;
        Vector3 out;
        in.x = 0;
        in.y = 0;
        in.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        if (mOpenPhase != 0) {
            if (mOpenPhase == 1) in.z = 0x2000;
        } else {
            in.y = -0x1000;
        }
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MulVec3Mat4x3(&in, &data_020a0e68, &out);
        AddVec3((Vector3 *)&mPosX, &out, (Vector3 *)&mPosX);
        if (Vec3_Dist(&mHomePosX, &mPosX) > 0xa000) {
            mOpenPhase = 1;
        }
        if (Vec3_HorzDist(&mHomePosX, &mPosX) > 0xc8000) {
            CannonHatch *a = _ZN8dActor_c15FindWithActorIDEjPS_(0xe, 0);
            while (a != 0) {
                a->mCannonOpen = 1;
                a = _ZN8dActor_c15FindWithActorIDEjPS_(0xe, a);
            }
        }
        _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
    }
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0)) {
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
    }
    return 1;
}
