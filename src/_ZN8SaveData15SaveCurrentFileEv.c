#include "types.h"
/* _ZN8SaveData15SaveCurrentFileEv at 0x02013b9c
 * Saves the current file and minigame data.
 */
struct MinigameSaveData { char data[0x2e4]; };

extern u8 data_0209caa0[];               /* 0x0209caa0 */
extern struct MinigameSaveData data_0209cae4;  /* 0x0209cae4 = &data_0209caa0[0x44] */

extern int _ZN8SaveData8SaveFileEjP12FileSaveData(u32 fileID, u8* data);
extern int _ZN8SaveData13SaveMinigamesEP16MinigameSaveData(struct MinigameSaveData* data);

int _ZN8SaveData15SaveCurrentFileEv(void)
{
    if (!_ZN8SaveData8SaveFileEjP12FileSaveData(data_0209caa0[0x328], data_0209caa0))
        return 0;
    return _ZN8SaveData13SaveMinigamesEP16MinigameSaveData(&data_0209cae4);
}
