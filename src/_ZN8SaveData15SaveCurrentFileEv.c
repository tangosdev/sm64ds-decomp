#include "types.h"
/* _ZN8SaveData15SaveCurrentFileEv at 0x02013b9c
 * Saves the current file and minigame data.
 */
struct MinigameSaveData { char data[0x2e4]; };

extern u8 SAVE_DATA[];               /* 0x0209caa0 */
extern struct MinigameSaveData SAVE_MG_DATA;  /* 0x0209cae4 = &SAVE_DATA[0x44] */

extern int _ZN8SaveData8SaveFileEjP12FileSaveData(u32 fileID, u8* data);
extern int _ZN8SaveData13SaveMinigamesEP16MinigameSaveData(struct MinigameSaveData* data);

int _ZN8SaveData15SaveCurrentFileEv(void)
{
    if (!_ZN8SaveData8SaveFileEjP12FileSaveData(SAVE_DATA[0x328], SAVE_DATA))
        return 0;
    return _ZN8SaveData13SaveMinigamesEP16MinigameSaveData(&SAVE_MG_DATA);
}
