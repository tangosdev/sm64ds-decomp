//cpp
#include "SnowmanHead.h"

extern "C" {
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
    void *self, const void *pos, int magnitude);
int func_0201267c(int id, void *pos);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    unsigned int id, int x, int y, int z);
int _ZN5Sound7PlaySubEjjj5Fix12IiEb(
    unsigned int soundID, unsigned int volume, unsigned int pan,
    int distance, int loop);
unsigned char DecIfAbove0_Byte(unsigned char *value);
}
struct SnowmanHeadVec3 {
    s32 x;
    s32 y;
    s32 z;
};

int SnowmanHead::State2()
{
    SnowmanHeadVec3 pos;
    switch (mSubstate) {
    case 0:
        UpdatePos(&mCylinder);
        if (mVertSpeed < 0 && mPosY < (int)0xffc427c0) {
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
                this, &pos, 0x5dc000);
            func_0201267c(0x115, (char *)this + 0x74);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0x10f, mPosX, mPosY, mPosZ);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0x110, mPosX, mPosY, mPosZ);
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(
                0x20, 0x14, 0x7f, 0x15666, 0);
            mSubstate++;
        }
        break;
    case 1:
        if (DecIfAbove0_Byte(&mStateTimer) == 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(
                0x20, 0x7f, 0, 0x15666, 0);
            SetState(3);
        }
        break;
    }
    return 1;
}
