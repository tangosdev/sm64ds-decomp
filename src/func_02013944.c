#include "types.h"
typedef struct FileSaveData {
    u32 magic8000;
    u32 flags1;
    u32 flags2;
} FileSaveData;

extern FileSaveData SAVE_DATA;
extern u32 _ZN8SaveData22NumGlowingRabbitsFoundEv(void);

void func_02013944(void) {
    u32 count = _ZN8SaveData22NumGlowingRabbitsFoundEv();
    SAVE_DATA.flags2 = SAVE_DATA.flags2 | (0x100000u << count);
    if ((s32)count >= 7) {
        SAVE_DATA.flags1 = SAVE_DATA.flags1 | 0x80u;
    }
}
