#include "types.h"
/* NumStars at 0x0201367c
 * Counts total stars collected across all levels and all star slots.
 */
extern int IsStarCollected(s8 courseID, s32 starID);

u8 NumStars(void)
{
    u8 count = 0;
    s8 level = 0;
    s32 star;
    for (level = 0; level < 0x1e; level++) {
        for (star = 0; star < 8; star++) {
            if (IsStarCollected(level, star))
                count++;
        }
    }
    return count;
}
