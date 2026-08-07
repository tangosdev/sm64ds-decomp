//cpp
// @symbol _ZN8SaveData26CountStarsCollectedInLevelEj
#include "SaveData.h"

/* SaveData::CountStarsCollectedInLevel(u32 courseID) at 0x02013768 -- static.
 *
 * Counts this course's collected stars by asking IsStarCollected for each of the
 * eight star slots. The loop bound is 8 and not NUM_STARS-per-course because the
 * save layout reserves eight bits per course whether the level uses them or not.
 */
extern "C" int IsStarCollected(s32 courseID, s32 starID);

u8 SaveData::CountStarsCollectedInLevel(u32 courseID)
{
    u8 count = 0;
    s32 star;
    for (star = 0; star < 8; star++) {
        if (IsStarCollected((s32)courseID, star))
            count++;
    }
    return count;
}
