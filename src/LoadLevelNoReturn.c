#include "types.h"
extern void LoadLevel(u32 levelID, u32 arg1, u32 arg2, u32 arg3, u32 arg4);

extern s8 RETURN_LEVEL_ID;
extern s8 CHECKPOINT_LEVEL_ID;

void LoadLevelNoReturn(u32 levelID, u32 arg1, u32 arg2, u32 arg3)
{
    LoadLevel(levelID, arg1, arg2, arg3, (u32)-1);
    RETURN_LEVEL_ID = -1;
    CHECKPOINT_LEVEL_ID = -1;
}
