//cpp
// @symbol _ZN8SaveData16EraseAllSaveDataEv
#include "SaveData.h"

/* SaveData::EraseAllSaveData() at 0x02013e0c -- static, no `this`.
 *
 * Erases all three file slots and resets the minigame block. The three erases are
 * OR-ed rather than short-circuited, so every slot is attempted even if an earlier
 * one fails -- and because EraseSaveFile returns 1 for success, a nonzero result
 * means at least one slot was written, not that something went wrong.
 *
 * HONEST LEFTOVER: SetDefaultValuesMg is called by its raw mangled name -- see
 * _ZN8SaveData16ReadMinigameDataEP16MinigameSaveData.c for why.
 */
extern "C" {
extern unsigned char data_0209caa0[];
void _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData(MinigameSaveData* mg);
}

u32 SaveData::EraseAllSaveData()
{
    u32 r4;
    r4 = SaveData::EraseSaveFile(0, (char*)data_0209caa0);
    r4 |= SaveData::EraseSaveFile(1, (char*)data_0209caa0);
    r4 |= SaveData::EraseSaveFile(2, (char*)data_0209caa0);
    _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData((MinigameSaveData*)(data_0209caa0 + 0x44));
    return r4 | SaveData::SaveMinigames((MinigameSaveData*)(data_0209caa0 + 0x44));
}
