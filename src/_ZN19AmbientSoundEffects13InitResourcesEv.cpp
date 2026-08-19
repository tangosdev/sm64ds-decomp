//cpp
// @symbol _ZN19AmbientSoundEffects13InitResourcesEv
#include "AmbientSoundEffects.h"
extern "C" int IsStarCollectedInLevel(s8 levelID, int starID);
extern s8 data_0209f2f8;
extern u8 data_0209f220;
extern u8 data_0209f2d8;
extern int data_0209caa0[];
extern int data_0209fc48;
extern int data_ov002_02110aec;

int AmbientSoundEffects::InitResources()
{
    int flag;

    if (data_0209f2f8 == 8 && (data_0209f220 == 1 || IsStarCollectedInLevel(8, 1) == 0))
        return 0;

    /* Volatile keeps CW from materializing an extra base pointer for this
       inherited-field RMW; the ROM loads and stores [this + 0x08] directly. */
    u32 soundID = *(volatile u32 *)((char *)this + 0x8);
    *(volatile u32 *)((char *)this + 0x8) = soundID & 0xf;

    if ((int)(data_0209f2d8 == 0) != 0
        && (data_0209caa0[2] & 0x80) == 0
        && (int)(data_0209fc48 != 0) == 0)
        flag = 1;
    else
        flag = 0;

    data_ov002_02110aec = flag;
    return 1;
}
