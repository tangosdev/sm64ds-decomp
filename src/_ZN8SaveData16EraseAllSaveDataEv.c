#include "types.h"
/* _ZN8SaveData16EraseAllSaveDataEv at 0x02013e0c
 * Erases all three save files and resets minigame data.
 * Returns OR of all operation results (0 = all succeeded).
 */

extern unsigned char SAVE_DATA[];
extern u32 _ZN8SaveData13EraseSaveFileEjPc(u32 fileID, char *saveArea);
extern void _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData(void *mgData);
extern u32 _ZN8SaveData13SaveMinigamesEP16MinigameSaveData(void *mgData);

u32 _ZN8SaveData16EraseAllSaveDataEv(void)
{
    u32 r4;
    r4 = _ZN8SaveData13EraseSaveFileEjPc(0, (char *)SAVE_DATA);
    r4 |= _ZN8SaveData13EraseSaveFileEjPc(1, (char *)SAVE_DATA);
    r4 |= _ZN8SaveData13EraseSaveFileEjPc(2, (char *)SAVE_DATA);
    _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData((void *)(SAVE_DATA + 0x44));
    return r4 | _ZN8SaveData13SaveMinigamesEP16MinigameSaveData((void *)(SAVE_DATA + 0x44));
}
