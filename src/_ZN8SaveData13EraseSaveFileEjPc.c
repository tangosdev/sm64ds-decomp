#include "types.h"
/* _ZN8SaveData13EraseSaveFileEjPc at 0x02013cd4
 * Erases a save file slot by writing defaults then saving to cart.
 * Returns 1 on success, 0 on failure.
 */
struct FileSaveData { char data[0x44]; };

extern void _ZN8SaveData16SetDefaultValuesEP12FileSaveData(struct FileSaveData* data);
extern int _ZN8SaveData14SaveDataToCartEPcjj(char* data, u32 size, u32 fileID);

int _ZN8SaveData13EraseSaveFileEjPc(u32 fileID, char* saveArea)
{
    _ZN8SaveData16SetDefaultValuesEP12FileSaveData((struct FileSaveData*)saveArea);
    if (_ZN8SaveData14SaveDataToCartEPcjj(saveArea, 0x44, fileID) == 0)
        return 1;
    return 0;
}
