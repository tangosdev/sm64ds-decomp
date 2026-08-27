#include "types.h"
extern u8 _ZN8SaveData26CountStarsCollectedInLevelEj(u32 courseID);

u8 CountStarsCollectedInLevelToDisplay(s32 courseID) {
    u8 total;
    s32 course;
    if (courseID <= 0xe) {
        return _ZN8SaveData26CountStarsCollectedInLevelEj((u32)courseID);
    }
    total = 0;
    course = 0xf;
    do {
        total = (total + _ZN8SaveData26CountStarsCollectedInLevelEj((u32)course)) & 0xff;
        course++;
    } while (course <= 0x1d);
    return total;
}
