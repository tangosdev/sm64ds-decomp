//cpp
// @symbol _ZN8SaveData16CanPlayerHaveCapEv
#include "SaveData.h"

/* SaveData::CanPlayerHaveCap() at 0x02013b5c -- static, no `this`.
 *
 * True when the current character can wear a cap at all: character 3 (Wario)
 * never can, and a second global at 0x0209f2d8 gates it besides. Both are read
 * as raw globals because neither has a friendly symbol yet; the byte at +0x41
 * of the save block is the current character, which SaveData.h names mCharacter.
 */
extern "C" {
extern unsigned char data_0209caa0[];
extern unsigned char data_0209f2d8[];
}

int SaveData::CanPlayerHaveCap()
{
    if (data_0209caa0[0x41] != 3) {
        int b = (int)(data_0209f2d8[0] == 1);
        if (b == 0)
            return 1;
    }
    return 0;
}
