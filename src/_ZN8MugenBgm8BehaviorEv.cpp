//cpp
#include "types.h"
// @symbol _ZN8MugenBgm8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MugenBgm.h"
extern "C" {
void Vec3_Sub(int* out, int* a, int* b);
void Vec3_RotateYAndTranslate(int* out, int* in, short angle, int* src);
u32 NumStars(void);
int AngleDiff(int a, int b);
void _ZN3Fog4InitEt5Fix12IiES1_(void* thiz, unsigned short a, int b, int d);
int func_ov100_02144f84(void);
int _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, int d, int e);
}

extern u8 data_0209f250;
extern char* data_0209f394[];
extern int data_020a0ebc[];
extern u8 data_0209caa0[];
extern char* data_0209f318;

int MugenBgm::Behavior()
{
    int vol;
    int flag;
    int depth;
    int color;
    int diff[3];
    int rotated[3];

    Vec3_Sub(diff, (int*)(data_0209f394[data_0209f250] + 0x5c), (int*)&mPosX);
    Vec3_RotateYAndTranslate(rotated, data_020a0ebc, mAngleY, diff);

    if (param1 == 1) {
        int z = rotated[2];
        int x = rotated[0];
        int az = (z < 0) ? -z : z;
        if (x < 0) x = -x;
        if (x < 0x320000) {
            int y = rotated[1];
            if (y > -0x12c000 && y < 0x5dc000 && az > 0x32000 && az < 0xc8000)
                data_0208e434 = (z < 0) ? 2 : -1;
        }
        return 1;
    }

    depth = 0xffff;
    vol = 0x7f;
    color = depth;
    flag = 0;

    if (NumStars() < 0x50 || data_0209caa0[0x41] != 0) {
        int x = rotated[0];
        color = 0xd00;
        if (x < 0) x = -x;
        if (x < 0x200000) {
            int y = rotated[1];
            if (y > -0x32000 && y < 0x1000000 && rotated[2] < 0) {
                int d;
                vol = 0;
                flag = 0x7f;
                d = AngleDiff(mAngleY, *(s16*)(data_0209f318 + 0x17c)) - 0x2000;
                if (d < 0)
                    d = 0;
                else if (d > 0x4000)
                    d = 0x4000;
                depth = (int)(((s64)rotated[2] * (0x40 - (d >> 8)) + 0x800) >> 12) + 0xffff;
                if (depth < 0x700)
                    depth = 0x700;
            }
        }
    }

    {
        void* fog = data_ov002_02110af8;
        if (fog != 0)
            _ZN3Fog4InitEt5Fix12IiES1_(fog, 0, (u16)depth, color);
    }

    if (mHorzSpeed == 0) {
        if (func_ov100_02144f84() == 0)
            return 1;
        mHorzSpeed = 1;
    }

    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x2e, vol, flag, 0x1451, 0) != 0) {
        if (flag == 0)
            mHorzSpeed = 0;
    }
    return 1;
}
