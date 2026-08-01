#include "types.h"
// @symbol _ZN8SaveData19IsCharacterUnlockedEj
/* recovered: named members + shared header */
#include "SaveData.h"
/* SaveData::IsCharacterUnlocked(u32 character) at 0x0201392c
 * Static method (no `this`); returns nonzero if the character's bit is set.
 * Tests bit `character` in the u32 data_0209caa0.flags2 (the FileSaveData word at
 * +0x8). Mirrors SetCharacterIntro. See Save.h flags2.
 *
 * The reloc resolves to data_0209caa0 base 0x0209caa0; flags2 is index [2]
 * (offset +0x8). Friendly symbol not yet in symbols.txt (wildcard pooled
 * reloc, name not byte-verified). The [2] index and `1 << n` shift are
 * load-bearing.
 */
extern s32 data_0209caa0[]; /* &data_0209caa0 as s32[]; [2] == flags2, base 0x0209caa0 */

s32 _ZN8SaveData19IsCharacterUnlockedEj(u32 character)
{
    return data_0209caa0[2] & (1 << character); /* flags2 & (1 << character) */
}
