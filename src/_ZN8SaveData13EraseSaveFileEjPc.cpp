//cpp
// @symbol _ZN8SaveData13EraseSaveFileEjPc
#include "SaveData.h"

/* SaveData::EraseSaveFile(u32 fileID, char* saveArea) at 0x02013cd4 -- static.
 *
 * Resets one file slot to defaults in `saveArea`, then writes it to cart.
 * Returns 1 on success, inverting SaveDataToCart's 0-is-success convention.
 *
 * SetDefaultValues is a real static call now: the ROM body takes one pointer in
 * r0 and this call passes saveArea into it.
 */

int SaveData::EraseSaveFile(u32 fileID, char* saveArea)
{
    SaveData::SetDefaultValues((FileSaveData*)saveArea);
    if (SaveData::SaveDataToCart(saveArea, 0x44, fileID) == 0)
        return 1;
    return 0;
}
