//cpp
// @symbol _ZN8SaveData16SetDefaultValuesEP12FileSaveData
#include "decl_common.h"
#include "SaveData.h"

/* SaveData::SetDefaultValues(FileSaveData*) at 0x02013dc4.
 *
 * Zeroes the 0x44-byte file block, then stamps the "8000" magic, sets the current
 * character to 3, sets bit 3 of flags2 and clears unk_042.
 *
 * STATIC, and the parameter is the pointer the ROM reads in r0. This file used to
 * model that pointer as `this` and carry `fsd_` unread, purely to spell the
 * mangled name `EP12FileSaveData`, with a note claiming that correcting the
 * declaration would move the argument out of `this` and change the bytes. It does
 * not: the mangled name, the ROM body and all four callers agree that the single
 * r0 argument IS the FileSaveData*, and spelling it that way reproduces the
 * function byte for byte. EraseSaveFile, ReadFileData and EraseAllSaveData now
 * make real static calls instead of reaching for the raw mangled name.
 *
 * The read-modify-write on the word at +8 keeps its materialised address form --
 * `add r2, r4, #8` then ldr/orr/str through r2 -- which is what the offset
 * expression below reproduces. See plan-cpp-language-mode.md Phase 6.
 *
 * That 32-bit read-modify-write is one of the three accesses that proved flags2
 * is s32 and not the u8 the header used to declare.
 */
void SaveData::SetDefaultValues(FileSaveData* fsd_)
{
    func_0205a588(((void*)fsd_), 0, 0x44);
    *(int*)((void*)fsd_) = 0x30303038;      /* magic8000 */
    *(unsigned char*)((char*)fsd_ + 0x41) = 3;
    *(int*)((char*)fsd_ + 8) |= 8;
    *(unsigned char*)((char*)fsd_ + 0x42) = 0;
}
