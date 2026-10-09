//cpp
// @symbol _ZN8SaveData19IsCharacterUnlockedEj
#include "SaveData.h"

/* SaveData::IsCharacterUnlocked(u32 character) at 0x0201392c -- static, no `this`.
 *
 * Tests bit `character` in flags2 (+0x08), the same word SetCharacterIntro writes.
 * The 32-bit width and the `1 << n` shift are load-bearing -- this used to be
 * spelled `data_0209caa0[2]` over an s32[], which is what proved flags2 is 32 bits
 * wide and not the u8 the header used to declare.
 *
 * 0x0209caa0 has no friendly symbol yet and the reloc is a wildcard pooled global,
 * so the extern NAME is not byte-verified; the width and shift are.
 */
extern "C" struct SaveData data_0209caa0;

s32 SaveData::IsCharacterUnlocked(u32 character)
{
    return data_0209caa0.flags2 & (1 << character);
}
