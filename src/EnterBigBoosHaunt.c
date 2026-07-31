#include "types.h"
extern void LoadLevel(u32 levelID, u32 arg1, u32 arg2, u32 arg3, u32 arg4);

void EnterBigBoosHaunt(void)
{
    LoadLevel(0xc, 0, 0, 0, 0);
}
