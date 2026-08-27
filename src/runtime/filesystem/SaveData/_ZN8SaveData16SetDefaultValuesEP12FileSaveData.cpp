//cpp
// @symbol _ZN8SaveData16SetDefaultValuesEP12FileSaveData
#include "decl_common.h"
#include "SaveData.h"

/* SaveData::SetDefaultValues(FileSaveData*) at 0x02013dc4.
 *
 * Zeroes the 0x44-byte file block, then stamps the "8000" magic, sets the current
 * character to 3, sets bit 3 of flags2 and clears unk_042.
 *
 * NOT STATIC, and the declared parameter is a fiction. The ROM function takes ONE
 * pointer in r0 and this models it as `this`, so `fsd_` is never read -- it exists
 * only to spell the mangled name `EP12FileSaveData`. That is why static callers
 * (EraseSaveFile, ReadFileData, ReadMinigameData) still reach it by its raw
 * mangled name rather than as a real call: correcting the declaration to a static
 * taking FileSaveData* would move the argument out of `this` and change the bytes.
 * Settling that signature is its own change.
 *
 * The `long long` cast on the flags2 store is a codegen hack, not meaning -- it
 * forces address materialisation. See plan-cpp-language-mode.md Phase 6.
 *
 * The 32-bit read-modify-write below is one of the three accesses that proved
 * flags2 is s32 and not the u8 the header used to declare.
 */
void SaveData::SetDefaultValues(FileSaveData* fsd_)
{
    func_0205a588(((void*)this), 0, 0x44);
    *(int*)((void*)this) = 0x30303038;      /* magic8000, via `this` */
    *(unsigned char*)((char*)&mCharacter) = 3;
    *(int*)((char*)&flags2) |= 8;
    *(unsigned char*)((char*)&unk_042) = 0;
}
