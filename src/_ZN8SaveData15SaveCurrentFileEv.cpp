//cpp
// @symbol _ZN8SaveData15SaveCurrentFileEv
#include "SaveData.h"

/* SaveData::SaveCurrentFile() at 0x02013b9c -- static, no `this`.
 *
 * Writes the active file slot and then the minigame block, short-circuiting: if
 * the file write fails there is no point writing minigames. The active slot index
 * is the byte at +0x328 of the save block, and the minigame block begins at +0x44.
 *
 * 0x0209caa0 has no friendly symbol yet, so the block is reached as a raw byte
 * array and the two offsets are spelled out rather than named.
 */
extern "C" u8 data_0209caa0[]; /* base of the save block */

int SaveData::SaveCurrentFile()
{
    if (!SaveData::SaveFile(data_0209caa0[0x328], (FileSaveData*)data_0209caa0))
        return 0;
    return SaveData::SaveMinigames((MinigameSaveData*)(data_0209caa0 + 0x44));
}
