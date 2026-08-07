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
 * HONEST LEFTOVER: SetDefaultValuesMg is still called by its raw mangled name.
 * SaveData.h declares it NON-static and its body uses `this` while ignoring its
 * declared parameter -- so the ROM's single r0 argument is modelled as `this` and
 * the explicit parameter is a fiction that exists to spell the mangled name. A
 * static context cannot call that, and correcting the declaration would change
 * that function's own codegen. Migration is per-reference; this reference waits
 * on SetDefaultValues/SetDefaultValuesMg being settled.
 */
extern "C" void _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData(MinigameSaveData* mg);

int SaveData::ReadMinigameData(MinigameSaveData* dest)
{
    s32 result = SaveData::ReadDataFromCart((char*)dest, 0x2e4, 3);
    if (result) {
        _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData(dest);
        if (result == 2)
            return 1;
        return 0;
    }
    return 1;
}
