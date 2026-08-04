//cpp
#include "types.h"
// @symbol _ZN19AmbientSoundEffects13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "AmbientSoundEffects.h"
extern int IsStarCollectedInLevel(s8 levelID, int starID);
extern s8 data_0209f2f8;
extern u8 data_0209f220;
extern u8 data_0209f2d8;
extern int data_0209caa0[];
extern int data_0209fc48;

int AmbientSoundEffects::InitResources()
{
    int flag;

    if (data_0209f2f8 == 8 && (data_0209f220 == 1 || IsStarCollectedInLevel(8, 1) == 0))
        return 0;

    unk_008 &= 0xf;

    if ((int)(data_0209f2d8 == 0) != 0
        && (data_0209caa0[2] & 0x80) == 0
        && (int)(data_0209fc48 != 0) == 0)
        flag = 1;
    else
        flag = 0;

    data_ov002_02110aec = flag;
    return 1;
}
