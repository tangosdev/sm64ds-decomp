// @symbol _ZN8SaveData26CountStarsCollectedInLevelEj
/* recovered: named members + shared header */
#include "SaveData.h"
/* _ZN8SaveData26CountStarsCollectedInLevelEj at 0x02013768
 * Counts how many stars have been collected in a given course (0-7 stars).
 */
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern int IsStarCollected(s32 courseID, s32 starID);

u8 _ZN8SaveData26CountStarsCollectedInLevelEj(u32 courseID)
{
    u8 count = 0;
    s32 star;
    for (star = 0; star < 8; star++) {
        if (IsStarCollected((s32)courseID, star))
            count++;
    }
    return count;
}
