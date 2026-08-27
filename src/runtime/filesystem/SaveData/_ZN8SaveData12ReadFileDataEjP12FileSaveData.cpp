//cpp
// @symbol _ZN8SaveData12ReadFileDataEjP12FileSaveData
#include "SaveData.h"

/* SaveData::ReadFileData(u32 fileID, FileSaveData* dest) at 0x02013d54 -- static.
 *
 * Reads one 0x44-byte file slot from cart, writing defaults if it could not be
 * read. Same three-way result as ReadMinigameData: 0 success, 2 fresh cart
 * (defaults, still success), 1 real failure.
 *
 * On success it masks the word at dest+0xc with the word at save-block +0x48 --
 * clearing bits the global block says are not available. The `long long` cast is
 * a codegen hack, not meaning: it forces the address into a register instead of
 * being folded, and removing it changes the instruction. See
 * plan-cpp-language-mode.md Phase 6.
 *
 * The local `struct FileSaveData { char data[0x44]; }` shadow is retired; the type
 * now comes from SaveData.h. HONEST LEFTOVER: SetDefaultValues keeps its raw
 * mangled call -- see _ZN8SaveData16ReadMinigameDataEP16MinigameSaveData.c.
 */
extern "C" {
void _ZN8SaveData16SetDefaultValuesEP12FileSaveData(FileSaveData* data);
extern int data_0209caa0;
}

int SaveData::ReadFileData(u32 fileID, FileSaveData* dest)
{
    char* r5 = (char*)dest;
    s32 result = SaveData::ReadDataFromCart(r5, 0x44, fileID);
    if (result) {
        _ZN8SaveData16SetDefaultValuesEP12FileSaveData((FileSaveData*)r5);
        if (result == 2)
            return 1;
        return 0;
    }
    {
        int* p = (int*)(r5 + 0xc);
        *p = *p & *(int*)((char*)&data_0209caa0 + 0x48);
    }
    return 1;
}
