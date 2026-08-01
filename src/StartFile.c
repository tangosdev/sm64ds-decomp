#include "types.h"
/* StartFile at 0x0202ae88
 * Starts a single-player game file: clears gamemode, loads level, sets player globals,
 * copies current character to data_02092128, sets player count to 1,
 * clears star-obtained/level flags, then starts a scene fade.
 */
extern u8 data_0209f2d8[];
extern u8 data_0209caa0[];
extern u8 data_02092128[];
extern u8 data_02092124[];
extern u8 data_0209f228[];

extern void LoadLevelNoReturn(s8 levelID, u8 entranceID, s8 starID, u8 returnState);
extern void SetPlayerGlobals(void);
extern void SetNumPlayers(u32 n);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 c);

void StartFile(s8 levelID, u8 entranceID)
{
    data_0209f2d8[0] = 0;
    LoadLevelNoReturn(levelID, entranceID, 1, 0);
    SetPlayerGlobals();
    data_02092128[0] = data_0209caa0[0x41];
    SetNumPlayers(1);
    data_02092124[0] = 6;
    data_0209f228[0] = 0;
    _ZN5Scene14StartSceneFadeEjjt(3, 0, 0x7fff);
}
