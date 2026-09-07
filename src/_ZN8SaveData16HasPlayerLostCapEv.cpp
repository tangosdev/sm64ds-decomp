//cpp
// @symbol _ZN8SaveData16HasPlayerLostCapEv
#include "SaveData.h"

/* SaveData::HasPlayerLostCap() at 0x02013b18 -- static, no `this`.
 *
 * The read side of PlayerLoseCap: tests bit 24 + currentCharacter in the flags
 * word at +0x04. Returns 0 for a character who cannot wear a cap, so "has not
 * lost it" and "could never have it" answer the same -- which is what the
 * callers want, since both mean "do not draw the capless head".
 */
extern "C" struct SaveData data_0209caa0;

int SaveData::HasPlayerLostCap()
{
    if (!SaveData::CanPlayerHaveCap())
        return 0;
    return data_0209caa0.flags1 & (0x1000000u << data_0209caa0.mCharacter);
}
