//cpp
// @symbol _ZN8SaveData13EraseSaveFileEjPc
#include "SaveData.h"

/* SaveData::EraseSaveFile(u32 fileID, char* saveArea) at 0x02013cd4 -- static.
 *
 * Resets one file slot to defaults in `saveArea`, then writes it to cart.
 * Returns 1 on success, inverting SaveDataToCart's 0-is-success convention.
 *
 * HONEST LEFTOVER: SetDefaultValues is called by its raw mangled name -- see
 * _ZN8SaveData16ReadMinigameDataEP16MinigameSaveData.c for why.
 */
extern "C" void _ZN8SaveData16SetDefaultValuesEP12FileSaveData(FileSaveData* data);

int SaveData::EraseSaveFile(u32 fileID, char* saveArea)
{
    _ZN8SaveData16SetDefaultValuesEP12FileSaveData((FileSaveData*)saveArea);
    if (SaveData::SaveDataToCart(saveArea, 0x44, fileID) == 0)
        return 1;
    return 0;
}
