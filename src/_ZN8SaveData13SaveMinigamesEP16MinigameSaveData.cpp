//cpp
// @symbol _ZN8SaveData13SaveMinigamesEP16MinigameSaveData
#include "SaveData.h"

/* SaveData::SaveMinigames(MinigameSaveData* data) at 0x02013be0 -- static.
 *
 * Writes the 0x2e4-byte minigame block to cart slot 3. Inverts the cart layer's
 * convention: SaveDataToCart returns 0 for success, this returns 1.
 */
int SaveData::SaveMinigames(MinigameSaveData* data)
{
    if (SaveData::SaveDataToCart((char*)data, 0x2e4, 3) == 0)
        return 1;
    return 0;
}
