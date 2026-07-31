#include "types.h"
// SetPlayerGlobals - initializes player globals: lives, health, controller mode
extern u8 NUM_LIVES;
extern u16 HEALTH_ARR[];
extern u8 SAVE_DATA[];
extern signed char NEXT_HAT_CHARACTER;
extern u8 UNK_YOSHI_EGG_RELATED;

extern void InitControllerMode(s32 playerIdx, s32 healthVal);

void SetPlayerGlobals(void) {
    NUM_LIVES = 4;
    s32 i = 0;
    do {
        u8 healthVal = SAVE_DATA[0x42];
        HEALTH_ARR[i] = 0x880;
        InitControllerMode(i, healthVal);
        i++;
    } while (i < 4);
    NEXT_HAT_CHARACTER = -1;
    UNK_YOSHI_EGG_RELATED = 0;
}