#include "types.h"
/* IsStarCollected at 0x020137e0
 * Reads the save-slot star bitmask for a course. The signature is the
 * one every decompiled caller declares: IsStarCollectedInLevel converts
 * its levelID to an s8 courseID before the call, NumStars walks an s8
 * loop. The s8 also makes a callee off-ARM extend the byte itself -- as
 * an int, a byte-laden register indexed data_0209cab4 wild on x86 (the
 * PC port faulted at data_0209cab4 + 0x75760a00 on its first lookup).
 */
extern unsigned char data_0209cab4[];
int IsStarCollected(s8 courseID, s32 starID)
{
    return ((1 << starID) & data_0209cab4[courseID]) & 0xff;
}
