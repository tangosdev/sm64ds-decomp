//cpp
// @symbol _ZN15FireSeaElevator8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "FireSeaElevator.h"
typedef int Fix12;
extern "C" {
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, Fix12 a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
}
extern short data_02082214[];

int FireSeaElevator::Behavior()
{
    _ZN12CylinderClsn5ClearEv((char *)&mMovingCylinderClsn);
    _ZN12CylinderClsn6UpdateEv((char *)&mMovingCylinderClsn);
    if (unk_008 != 0xffff) {
        int idx = unk_354 >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        *(int*)(((int)((char *)this) + 0x60)) =
            *(int*)(((int)((char *)this) + 0x60)) + (int)(((long long)s * 0x7000 + 0x800) >> 12);
    } else {
        int idx = unk_354 >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        *(int*)(((int)((char *)this) + 0x60)) =
            *(int*)(((int)((char *)this) + 0x60)) - (int)(((long long)s * 0x3000 + 0x800) >> 12);
    }
    *(short*)(((int)((char *)this) + 0x354)) =
        *(short*)(((int)((char *)this) + 0x354)) + 0x100;
    _ZN8Platform21UpdateModelPosAndRotYEv(((char *)this));
    if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(((char *)this), 0x400000, 0)) {
        _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
    }
    return 1;
}
