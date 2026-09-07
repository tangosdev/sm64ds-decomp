//cpp
// @symbol _ZN22ClockPaintingHandShort8BehaviorEv
/* recovered: real C++ method over inherited actor fields */
#include "decl_common.h"
#include "ClockPaintingHandShort.h"
extern "C" {
int IsAreaShowing(int areaId);
}
extern signed char data_02092110[];
extern unsigned char data_0209f2c0[];

int ClockPaintingHandShort::Behavior()
{
    if (data_02092110[0] <= 0) {
        mAngleZ += data_ov013_021116ac[mHandIndex];
    } else if (IsAreaShowing(mAreaId) && mHandIndex == 0) {
        unsigned short angle = (unsigned short)-mAngleZ;
        if (angle < 0x2000)
            data_0209f2c0[0] = 3;
        else if (angle < 0x6000 && angle >= 0x2000)
            data_0209f2c0[0] = 0;
        else if (angle < 0xa000 && angle >= 0x6000)
            data_0209f2c0[0] = 2;
        else if (angle < 0xe000 && angle >= 0xa000)
            data_0209f2c0[0] = 1;
        else if (angle >= 0xe000)
            data_0209f2c0[0] = 3;
    }
    func_ov013_02111430((char *)this);
    return 1;
}
