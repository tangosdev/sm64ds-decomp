//cpp
// @symbol _ZN8SaveData13PlayerLoseCapEv
#include "SaveData.h"

/* SaveData::PlayerLoseCap() at 0x02013ad4 -- static, no `this`.
 *
 * Records that the current character has lost their cap, by setting bit
 * 24 + currentCharacter in the flags word at +0x04 of the save block. Does
 * nothing for a character who cannot wear one.
 *
 * The save block is reached as a raw global: the reloc is a wildcard pooled
 * reference to 0x0209caa0 and no friendly symbol exists for it yet, so this
 * local view of the two fields it touches is deliberate rather than a shadow
 * of the whole class -- SaveData.h describes the object, this names the two
 * words the ROM instruction stream actually reads.
 */
struct SaveDataGlobal {
    u32 magic8000;         /* 0x00 */
    u32 flags1;            /* 0x04 -- bit 24+n: character n has lost their cap */
    char pad[0x39];
    u8  currentCharacter;  /* 0x41 */
};

extern "C" struct SaveDataGlobal data_0209caa0;

void SaveData::PlayerLoseCap()
{
    if (!SaveData::CanPlayerHaveCap())
        return;
    data_0209caa0.flags1 = data_0209caa0.flags1 | (0x1000000u << data_0209caa0.currentCharacter);
}
