//cpp
// @symbol _ZN8SaveData13PlayerLoseCapEv
#include "SaveData.h"

/* SaveData::PlayerLoseCap() at 0x02013ad4 -- static, no `this`.
 *
 * Records that the current character has lost their cap, by setting bit
 * 24 + currentCharacter in the flags word at +0x04 of the save block. Does
 * nothing for a character who cannot wear one.
 *
 * The save block still has no friendly symbol -- the reloc is a wildcard pooled
 * reference to 0x0209caa0 -- so it is reached by its placeholder name, but now
 * with its real type instead of a local shadow. `unk_041` is the current character.
 */
extern "C" struct SaveData data_0209caa0;

void SaveData::PlayerLoseCap()
{
    if (!SaveData::CanPlayerHaveCap())
        return;
    data_0209caa0.flags1 = data_0209caa0.flags1 | (0x1000000u << data_0209caa0.unk_041);
}
