// @symbol _ZN8SaveData17SetCharacterIntroEi
/* recovered: named members + shared header */
#include "SaveData.h"
/* SaveData::SetCharacterIntro(s32 character) at 0x02013910
 * Static method (no `this`); marks an intro/flag bit as seen.
 * Sets bit `character` in the u32 SAVE_DATA.flags2 (the FileSaveData word at
 * +0x8). Per Save.h flags2: bit 0 = Mario intro, bit 1 = Luigi intro,
 * bit 2 = Wario intro, ... etc.
 *
 * The reloc resolves to SAVE_DATA base 0x0209caa0; flags2 is the word at
 * index [2] (offset +0x8). The friendly symbol is not yet in symbols.txt
 * (wildcard pooled-global reloc, name not byte-verified). The [2] index and
 * `1 << n` shift are load-bearing -- changing either breaks the match.
 */

typedef int s32;

extern s32 SAVE_DATA[]; /* &SAVE_DATA as s32[]; [2] == flags2, base 0x0209caa0 */

void _ZN8SaveData17SetCharacterIntroEi(s32 character)
{
    SAVE_DATA[2] |= 1 << character; /* flags2 |= 1 << character */
}
