// @symbol _ZN8SaveData13SaveMinigamesEP16MinigameSaveData
/* recovered: named members + shared header */
#include "SaveData.h"
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;
typedef int s32;

struct MinigameSaveData;

extern int _ZN8SaveData14SaveDataToCartEPcjj(void* data, u32 size, u32 count);

int _ZN8SaveData13SaveMinigamesEP16MinigameSaveData(struct MinigameSaveData* data) {
    if (_ZN8SaveData14SaveDataToCartEPcjj(data, 0x2e4, 3) == 0)
        return 1;
    return 0;
}
