//cpp
// @symbol _ZN8SaveData8SaveFileEjP12FileSaveData
#include "SaveData.h"

/* SaveData::SaveFile(u32 fileID, FileSaveData* data) at 0x02013d14 -- static.
 *
 * Sets bit 0 of the word at data+0x4 -- marking the slot as in use -- then writes
 * the 0x44-byte block to cart, inverting SaveDataToCart's 0-is-success result.
 *
 * The `long long` cast is a codegen hack rather than meaning: it forces the
 * address into a register instead of being folded into the store. Removing it
 * changes the instruction. See plan-cpp-language-mode.md Phase 6.
 *
 * This file previously declared its own `struct SaveData` and a two-field
 * `FileSaveData`; both are retired in favour of the real header. That local
 * `FileSaveData { int _00; int _04; }` was also wrong about the size -- the block
 * is 0x44 bytes, as the call below has always said.
 */
int SaveData::SaveFile(u32 fileID, FileSaveData* data)
{
    int* ip = (int*)(((long long)(int)((char*)data + 4)));
    *ip = *ip | 1;
    if (SaveData::SaveDataToCart((char*)data, 0x44, fileID) == 0)
        return 1;
    return 0;
}
