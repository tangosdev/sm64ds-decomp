#include "types.h"
/* _ZN8SaveData16EraseAllSaveDataEv at 0x02013e0c
 * Erases all three save files and resets minigame data.
 * Returns OR of all operation results (0 = all succeeded).
 */

extern unsigned char data_0209caa0[];
extern u32 _ZN8SaveData13EraseSaveFileEjPc(u32 fileID, char *saveArea);
extern void _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData(void *mgData);
extern u32 _ZN8SaveData13SaveMinigamesEP16MinigameSaveData(void *mgData);

u32 _ZN8SaveData16EraseAllSaveDataEv(void)
{
    u32 r4;
    r4 = _ZN8SaveData13EraseSaveFileEjPc(0, (char *)data_0209caa0);
    r4 |= _ZN8SaveData13EraseSaveFileEjPc(1, (char *)data_0209caa0);
    r4 |= _ZN8SaveData13EraseSaveFileEjPc(2, (char *)data_0209caa0);
    _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData((void *)(data_0209caa0 + 0x44));
    return r4 | _ZN8SaveData13SaveMinigamesEP16MinigameSaveData((void *)(data_0209caa0 + 0x44));
}
