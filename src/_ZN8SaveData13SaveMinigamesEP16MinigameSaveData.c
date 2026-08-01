#include "types.h"
// @symbol _ZN8SaveData13SaveMinigamesEP16MinigameSaveData
/* recovered: named members + shared header */
#include "SaveData.h"
struct MinigameSaveData;

extern int _ZN8SaveData14SaveDataToCartEPcjj(void* data, u32 size, u32 count);

int _ZN8SaveData13SaveMinigamesEP16MinigameSaveData(struct MinigameSaveData* data) {
    if (_ZN8SaveData14SaveDataToCartEPcjj(data, 0x2e4, 3) == 0)
        return 1;
    return 0;
}
