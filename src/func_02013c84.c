#include "types.h"
struct FileSaveData {
    u32 magic8000;
    u32 flags1;
    u32 flags2;
    u32 minigameRabbits;
    u32 cannonUnlocked;
    u8  stars[30];
    u8  coinRecords[15];
    u8  currentCharacter;
    u8  controllerMode;
    u8  unk43;
};

typedef struct SaveDataGlobal {
    u8 pad[0x328];
    u8 unk328;
} SaveDataGlobal;

extern void CpuCopy8(void* dst, void* src, u32 count);
extern int _ZN8SaveData8SaveFileEjP12FileSaveData(u32 fileID, struct FileSaveData* saveData);
extern SaveDataGlobal data_0209caa0; /* 0x0209caa0 */

int func_02013c84(u8 charID, struct FileSaveData* dest, s32 fileIndex, struct FileSaveData* src) {
    CpuCopy8(dest, src, 0x44);
    if (fileIndex < 0) {
        data_0209caa0.unk328 = charID;
        return 1;
    }
    return _ZN8SaveData8SaveFileEjP12FileSaveData(fileIndex, src);
}
