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
struct SaveDataGlobal {
    u32 magic8000;         /* 0x00 */
    u32 flags1;            /* 0x04 -- bit 24+n: character n has lost their cap */
    char pad[0x39];
    u8  currentCharacter;  /* 0x41 */
};

extern "C" struct SaveDataGlobal data_0209caa0;

int SaveData::HasPlayerLostCap()
{
    if (!SaveData::CanPlayerHaveCap())
        return 0;
    return data_0209caa0.flags1 & (0x1000000u << data_0209caa0.currentCharacter);
}
