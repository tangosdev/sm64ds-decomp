//cpp
// @symbol _ZN8SaveData22NumGlowingRabbitsFoundEv
#include "SaveData.h"

/* SaveData::NumGlowingRabbitsFound() at 0x02013984 -- static, no `this`.
 *
 * Counts set bits 20..27 of the flags word at +0x08 -- the eight glowing rabbits.
 * The mask starts at 0x100000 (bit 20) and the loop runs exactly 8 times, so the
 * rabbits occupy the byte-aligned field above the character-unlock bits that
 * IsCharacterUnlocked and SetCharacterIntro use in the same word.
 *
 * The do/while shape is load-bearing: a for-loop tested at the top compiles to a
 * different prologue. 0x0209caa0 has no friendly symbol yet.
 */
extern "C" unsigned char data_0209caa0[];

int SaveData::NumGlowingRabbitsFound()
{
    int count = 0;
    int f = *(int*)(data_0209caa0 + 8);
    unsigned int mask = 0x100000;
    int i = 0;
    do {
        i++;
        if (f & mask)
            count++;
        mask <<= 1;
    } while (i < 8);
    return count;
}
