#include "types.h"
/* PrepareVsMode at 0x0202ae2c
 * Sets up VS mode: marks current game mode, initializes player globals,
 * starts a scene fade, then sets save data default values.
 */
extern unsigned char data_0209f2d8[];
extern unsigned char data_0209caa0[];

extern void SetPlayerGlobals(void);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
extern void _ZN8SaveData16SetDefaultValuesEP12FileSaveData(void *saveData);

void PrepareVsMode(void)
{
    data_0209f2d8[0] = 1;
    SetPlayerGlobals();
    _ZN5Scene14StartSceneFadeEjjt(3, 0, 0x7fff);
    _ZN8SaveData16SetDefaultValuesEP12FileSaveData((void *)data_0209caa0);
}
