//cpp
#include "types.h"
// @symbol _ZN15TtcRotatingCube8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TtcRotatingCube.h"
extern "C" {
extern u16 DecIfAbove0_Short(u16 *p);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *v);
extern int _Z14ApproachLinearRsss(s16 *val, int target, int step);
extern int RandomIntInternal(int *seed);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
}

extern u8 data_0209f2c0;
extern int data_0209e650;

int TtcRotatingCube::Behavior()
{
    if (data_0209f2c0 != 3) {
        switch (unk_376) {
        case 0:
            if (DecIfAbove0_Short((u16 *)((char *)&mWaitTimer)) != 0)
                break;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x5b, ((char *)this) + 0x74);
            (*(u8 *)(((int)((char *)this) + 0x376)))++;
            mVertSpeed = -0x5000;
            break;
        case 1:
            *(int *)(((int)((char *)this) + 0xa8)) += 0x800;
            *(int *)(((int)((char *)this) + 0x370)) += mVertSpeed;
            if (unk_370 < 0)
                break;
            unk_370 = 0;
            mWaitTimer = 6;
            (*(u8 *)(((int)((char *)this) + 0x376)))++;
            break;
        case 2:
            if (DecIfAbove0_Short((u16 *)((char *)&mWaitTimer)) != 0)
                break;
            if (_Z14ApproachLinearRsss((s16 *)((char *)&mAngleZ), unk_378, 0x4b0) == 0)
                break;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x40, ((char *)this) + 0x74);
            unk_376 = 0;
            mWaitTimer = data_ov065_0211cfa4[data_0209f2c0];
            if (data_0209f2c0 == 2)
                mWaitTimer = (unsigned int)RandomIntInternal(&data_0209e650) % 7 * 0x14 + 5;
            *(s16 *)(((int)((char *)this) + 0x378)) += data_ov065_0211cfa8[unk_377];
            break;
        }
    }
    func_ov065_0211990c(((char *)this));
    func_ov065_02119794(((char *)this));
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0))
        func_ov065_021198a0(((char *)this));
    return 1;
}
