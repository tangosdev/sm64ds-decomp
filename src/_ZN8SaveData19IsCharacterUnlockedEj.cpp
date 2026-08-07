//cpp
// @symbol _ZN8SaveData19IsCharacterUnlockedEj
#include "SaveData.h"

/* SaveData::IsCharacterUnlocked(u32 character) at 0x0201392c -- static, no `this`.
 *
 * Tests bit `character` in the flags word at +0x08 of the save block, the same
 * word SetCharacterIntro writes. The [2] index and the `1 << n` shift are both
 * load-bearing: this is a 32-bit load, so widening or narrowing the view
 * changes the instruction.
 *
 * 0x0209caa0 has no friendly symbol yet and the reloc is a wildcard pooled
 * global, so the extern name is not byte-verified -- the index and shift are.
 */
extern "C" s32 data_0209caa0[]; /* base of the save block; [2] is flags2 (+0x08) */

s32 SaveData::IsCharacterUnlocked(u32 character)
{
    return data_0209caa0[2] & (1 << character);
}
