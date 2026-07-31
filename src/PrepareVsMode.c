#include "types.h"
/* PrepareVsMode at 0x0202ae2c
 * Sets up VS mode: marks current game mode, initializes player globals,
 * starts a scene fade, then sets save data default values.
 */
extern unsigned char CURRENT_GAMEMODE[];
extern unsigned char SAVE_DATA[];

extern void SetPlayerGlobals(void);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
extern void _ZN8SaveData16SetDefaultValuesEP12FileSaveData(void *saveData);

void PrepareVsMode(void)
{
    CURRENT_GAMEMODE[0] = 1;
    SetPlayerGlobals();
    _ZN5Scene14StartSceneFadeEjjt(3, 0, 0x7fff);
    _ZN8SaveData16SetDefaultValuesEP12FileSaveData((void *)SAVE_DATA);
}
