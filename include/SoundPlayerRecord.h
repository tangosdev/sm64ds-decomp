/* Reconstructed from the users of the sound player table at sPlayer. */
#ifndef SOUNDPLAYERRECORD_H
#define SOUNDPLAYERRECORD_H
#include "types.h"
#include "nnsys/fnd.h"

/* One record of the 32-record table at sPlayer (stride 0x1c, ending at
   0x020a50ec). NNSi_SndPlayerInit initializes an NNSFndList at +0x00 and another
   at +0x0c and sets mPlayableSeqCount to 1; NNSi_SndPlayerAllocSeqPlayer compares the first
   list's count against it before it takes a voice, and
   Sound::Player::SetPlayableSeqCount writes it. The table's other users still
   declare it with shapes of their own. */
struct SoundPlayerRecord {
    NNSFndList mLists[2];           /* 0x000 */
    u32 mPlayableSeqCount;          /* 0x018 */
};

typedef char SoundPlayerRecord_size_must_be_0x1c[
    sizeof(struct SoundPlayerRecord) == 0x1c ? 1 : -1];

#endif
