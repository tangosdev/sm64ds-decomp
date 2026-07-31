#include "types.h"
/* StartFile at 0x0202ae88
 * Starts a single-player game file: clears gamemode, loads level, sets player globals,
 * copies current character to NEXT_HAT_CHARACTER_ARR, sets player count to 1,
 * clears star-obtained/level flags, then starts a scene fade.
 */
extern u8 CURRENT_GAMEMODE[];
extern u8 SAVE_DATA[];
extern u8 NEXT_HAT_CHARACTER_ARR[];
extern u8 LEVEL_OF_LAST_COLLECTED_STAR[];
extern u8 STAR_OBTAINED[];

extern void LoadLevelNoReturn(s8 levelID, u8 entranceID, s8 starID, u8 returnState);
extern void SetPlayerGlobals(void);
extern void SetNumPlayers(u32 n);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 c);

void StartFile(s8 levelID, u8 entranceID)
{
    CURRENT_GAMEMODE[0] = 0;
    LoadLevelNoReturn(levelID, entranceID, 1, 0);
    SetPlayerGlobals();
    NEXT_HAT_CHARACTER_ARR[0] = SAVE_DATA[0x41];
    SetNumPlayers(1);
    LEVEL_OF_LAST_COLLECTED_STAR[0] = 6;
    STAR_OBTAINED[0] = 0;
    _ZN5Scene14StartSceneFadeEjjt(3, 0, 0x7fff);
}
