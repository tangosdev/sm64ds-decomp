//cpp
// @symbol _ZN16RotatingCogSmall8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "RotatingCogSmall.h"
extern "C" {
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _Z14ApproachLinearRsss(short* dst, short to, short step);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern int RandomIntInternal(int* seed);
extern unsigned char data_0209f2c0[];
extern short data_ov035_02111ef4[][4];
extern short data_ov035_02111ef0[];
extern int data_0209e650[];
}

int RotatingCogSmall::Behavior()
{
    if (data_0209f2c0[0] == 3) {
        _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
        if (mRotationState == 0 && _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0))
            _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
        return 1;
    }

    if (_Z14ApproachLinearRsss((short*)((char*)&mAngleY), unk_322, 0xc8) != 0 &&
        DecIfAbove0_Short((unsigned short*)((char*)&unk_31e)) == 0) {
        short* p = (short*)(((int)((char*)this) + 0x322));
        *p = *p + unk_324;
        unsigned char k = data_0209f2c0[0];
        unk_31e = data_ov035_02111ef4[mRotationState][k];
        if (k == 2) {
            int rnd = RandomIntInternal(data_0209e650);
            if (DecIfAbove0_Short((unsigned short*)((char*)&unk_320)) == 0) {
                if ((unsigned int)rnd % 3 != 0) {
                    int r2 = (rnd & 3) * 0x3c;
                    unk_324 = data_ov035_02111ef0[mRotationState];
                    unk_320 = r2 + 0x5a;
                } else {
                    unk_324 = -data_ov035_02111ef0[mRotationState];
                    unk_320 = ((unsigned int)rnd % 3 + 1) * 0x1e;
                }
            }
            unk_31e = (unsigned int)rnd % 3 * 0x14 + 0xa;
        }
    }

    _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
    if (mRotationState == 0 && _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
    return 1;
}
