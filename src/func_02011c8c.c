#include "types.h"
/* func_02011c8c — resets all sound player sequence limits from a default table.
 * Iterates 10 entries of a static array {s32 playerID, s32 maxSeq} at 0x0208e448,
 * calling Sound::Player::SetPlayableSeqCount(playerID, maxSeq) for each.
 *
 * Callee: 0x0204fadc = Sound::Player::SetPlayableSeqCount(s32, s32)
 *   _ZN5Sound6Player19SetPlayableSeqCountEii
 */
extern void _ZN5Sound6Player19SetPlayableSeqCountEii(s32 playerID, s32 maxSeq);

typedef struct {
    s32 playerID;
    s32 maxSeq;
} SoundPlayerDefault;

extern SoundPlayerDefault data_0208e448[10];  /* 0x0208e448 */

void func_02011c8c(void)
{
    u32 i;
    for (i = 0; i < 10; i++)
        _ZN5Sound6Player19SetPlayableSeqCountEii(data_0208e448[i].playerID, data_0208e448[i].maxSeq);
}
