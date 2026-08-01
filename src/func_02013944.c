#include "types.h"
typedef struct FileSaveData {
    u32 magic8000;
    u32 flags1;
    u32 flags2;
} FileSaveData;

extern FileSaveData data_0209caa0;
extern u32 _ZN8SaveData22NumGlowingRabbitsFoundEv(void);

void func_02013944(void) {
    u32 count = _ZN8SaveData22NumGlowingRabbitsFoundEv();
    data_0209caa0.flags2 = data_0209caa0.flags2 | (0x100000u << count);
    if ((s32)count >= 7) {
        data_0209caa0.flags1 = data_0209caa0.flags1 | 0x80u;
    }
}
