#include "types.h"
/* DisableSoundPlayersForCredits — sets all 10 sound players' sequence limit to 0.
 * Iterates the same player-default table at 0x0208e448, but ignores the maxSeq
 * field and passes 0 instead, disabling each player.
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

void DisableSoundPlayersForCredits(void)
{
    u32 i;
    s32 zero;
    zero = 0;
    for (i = 0; i < 10; i++)
        _ZN5Sound6Player19SetPlayableSeqCountEii(data_0208e448[i].playerID, zero);
}
