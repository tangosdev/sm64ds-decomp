// @symbol _ZN8SaveData16ReadMinigameDataEP16MinigameSaveData
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_SaveData.h"
/* recovered: named members + shared header */
#include "SaveData.h"
/* _ZN8SaveData16ReadMinigameDataEP16MinigameSaveData at 0x02013c0c
 * Reads minigame save data from cart. Returns 1 on success, 0 on failure/no data.
 */

typedef unsigned int u32;
typedef unsigned char u8;
typedef signed int s32;

struct MinigameSaveData { char data[0x2e4]; };

extern void _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData(struct MinigameSaveData* data);

int _ZN8SaveData16ReadMinigameDataEP16MinigameSaveData(struct MinigameSaveData* dest)
{
    s32 r5 = (s32)dest;
    s32 result;
    result = _ZN8SaveData16ReadDataFromCartEPcjj((char*)r5, 0x2e4, 3);
    if (result) {
        _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData((struct MinigameSaveData*)r5);
        if (result == 2)
            return 1;
        return 0;
    }
    return 1;
}
