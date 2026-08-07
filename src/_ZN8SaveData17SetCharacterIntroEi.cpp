//cpp
// @symbol _ZN8SaveData17SetCharacterIntroEi
#include "SaveData.h"

/* SaveData::SetCharacterIntro(s32 character) at 0x02013910 -- static, no `this`.
 *
 * Marks a character's intro cutscene as seen, by setting bit `character` in the
 * flags word at +0x08 of the save block. IsCharacterUnlocked reads the same word:
 * bit 0 Mario, 1 Luigi, 2 Wario, and so on.
 *
 * The 32-bit width and the `1 << n` shift are load-bearing: this is a
 * read-modify-write of the whole word, which is what proves flags2 is s32 rather
 * than the u8 marker the header used to carry. 0x0209caa0 has no friendly symbol
 * yet, so the extern NAME is not byte-verified; the width and shift are.
 */
extern "C" struct SaveData data_0209caa0;

void SaveData::SetCharacterIntro(s32 character)
{
    data_0209caa0.flags2 |= 1 << character;
}
