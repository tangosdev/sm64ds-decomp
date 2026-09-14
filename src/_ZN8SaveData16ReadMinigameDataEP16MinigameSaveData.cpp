//cpp
// @symbol _ZN8SaveData16ReadMinigameDataEP16MinigameSaveData
#include "SaveData.h"

/* SaveData::ReadMinigameData(MinigameSaveData* dest) at 0x02013c0c -- static.
 *
 * Reads the 0x2e4-byte minigame block from cart slot 3, falling back to defaults
 * if the read failed. ReadDataFromCart's three return values are distinguished:
 *   0  read succeeded              -> return 1
 *   2  no valid data on the cart   -> write defaults, return 1 (a fresh cart is
 *                                     not an error)
 *   1  read genuinely failed       -> write defaults, return 0
 *
 * SetDefaultValuesMg is a real static call now. SaveData.h used to declare it
 * non-static, and its body modelled the ROM's single r0 argument as `this` while
 * ignoring the declared parameter; the mangled name, the ROM body and every
 * caller all say it is a static taking MinigameSaveData*, and spelling it that
 * way reproduces both bodies byte for byte.
 */

int SaveData::ReadMinigameData(MinigameSaveData* dest)
{
    s32 result = SaveData::ReadDataFromCart((char*)dest, 0x2e4, 3);
    if (result) {
        SaveData::SetDefaultValuesMg(dest);
        if (result == 2)
            return 1;
        return 0;
    }
    return 1;
}
