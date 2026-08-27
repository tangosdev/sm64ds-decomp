//cpp
// @symbol _ZN8Squasher8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Squasher.h"

struct Vec3 { int x, y, z; };

extern "C" {
int _ZN8dActor_c13DistToCPlayerEv(char *self);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, struct Vec3 *v);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, struct Vec3 *v, int f);
void AddVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
void func_0200fa04(void *c, void *v, int a);
void func_ov023_02111308(char *t);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
int func_ov023_0211124c(char *c);
}
extern short data_02082214[];

int Squasher::Behavior()
{
    switch (mState) {
    case 0:
        if (_ZN8dActor_c13DistToCPlayerEv((char *)this) < 0x3e8000) {
            mState++;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x43, (struct Vec3 *)&mCamSpacePosX);
        }
        break;
    case 1:
        mAngVelX -= 0x40;
        if (mAngVelX <= -0x1000) {
            mAngVelX = -0x1000;
        }
        mAngleX += mAngVelX;
        if (mAngleX <= -0x4000) {
            struct Vec3 off;
            struct Vec3 pos;
            int tmp[3];
            int s0, s1;

            mAngleX = -0x4000;
            mAngVelX = 0;
            mStateTimer = 0;

            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE((char *)this, &pos, 0x4000000);
            _ZN5Sound9PlayBank3EjRK7Vector3(0x44, (struct Vec3 *)&mCamSpacePosX);

            s0 = data_02082214[((u16)mAngleY >> 4) * 2];
            off.x = (int)(((long long)s0 * -0x190000 + 0x800) >> 12);
            s1 = data_02082214[((u16)mAngleY >> 4) * 2 + 1];
            off.z = (int)(((long long)s1 * -0x190000 + 0x800) >> 12);
            AddVec3(&off, (struct Vec3 *)&mPosX, &off);

            tmp[0] = off.x;
            tmp[1] = off.y;
            tmp[2] = off.z;
            func_0200fa04((char *)this, tmp, 1);

            mState++;
        }
        break;
    case 2:
        if ((u16)mStateTimer >= 0x3c) {
            mAngVelX = 0x80;
            mState++;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x45, (struct Vec3 *)&mCamSpacePosX);
        } else {
            (*(u16 *)&mStateTimer)++;
        }
        break;
    case 3:
        mAngleX += mAngVelX;
        if (mAngleX >= 0) {
            mAngVelX = 0;
            mAngleX = 0;
            mStateTimer = 0;
            mState++;
        }
        break;
    case 4:
        if ((u16)mStateTimer >= 0x3c) {
            mState = 0;
        } else {
            (*(u16 *)&mStateTimer)++;
        }
        break;
    }

    func_ov023_02111308((char *)this);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv((char *)this);
    func_ov023_0211124c((char *)this);
    return 1;
}
