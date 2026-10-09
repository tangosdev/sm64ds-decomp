#include "types.h"
extern void LoadLevel(u32 levelID, u32 arg1, u32 arg2, u32 arg3, u32 arg4);

extern s8 data_0209211c;
extern s8 data_02092118;

void LoadLevelNoReturn(u32 levelID, u32 arg1, u32 arg2, u32 arg3)
{
    LoadLevel(levelID, arg1, arg2, arg3, (u32)-1);
    data_0209211c = -1;
    data_02092118 = -1;
}
