//cpp
// @symbol _ZN18daObjKm2_Ami_Bou_c8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "daObjKm2_Ami_Bou_c.h"
extern "C" {
extern void _ZN5dCc_c5ClearEv(void *self);
extern void _ZN5dCc_c6UpdateEv(void *self);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
extern int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
}
extern short data_02082214[];

int daObjKm2_Ami_Bou_c::Behavior()
{
    _ZN5dCc_c5ClearEv((char *)&mdCcAc_c);
    _ZN5dCc_c6UpdateEv((char *)&mdCcAc_c);
    if ((*(s32 *)&param1) != 0xffff) {
        int idx = mHeightAng >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        *(int*)(((int)((char *)this) + 0x60)) =
            *(int*)(((int)((char *)this) + 0x60)) + (int)(((long long)s * 0x7000 + 0x800) >> 12);
    } else {
        int idx = mHeightAng >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        *(int*)(((int)((char *)this) + 0x60)) =
            *(int*)(((int)((char *)this) + 0x60)) - (int)(((long long)s * 0x3000 + 0x800) >> 12);
    }
    *(short*)(((int)((char *)this) + 0x354)) =
        *(short*)(((int)((char *)this) + 0x354)) + 0x100;
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char *)this));
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(((char *)this), 0x400000, 0)) {
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));
    }
    return 1;
}
