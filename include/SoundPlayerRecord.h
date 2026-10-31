/* Reconstructed from the users of the sound player table at data_020a4d6c. */
#ifndef SOUNDPLAYERRECORD_H
#define SOUNDPLAYERRECORD_H
#include "types.h"
#include "NestedHeapIterator.h"

/* One record of the 32-record table at data_020a4d6c (stride 0x1c, ending at
   0x020a50ec). func_0204fc40 builds a NestedHeapIterator at +0x00 and another
   at +0x0c and sets mPlayableSeqCount to 1; func_0204f63c compares the first
   list's count against it before it takes a voice, and
   Sound::Player::SetPlayableSeqCount writes it. The table's other users still
   declare it with shapes of their own. */
struct SoundPlayerRecord {
    NestedHeapIterator mLists[2];   /* 0x000 */
    u32 mPlayableSeqCount;          /* 0x018 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char SoundPlayerRecord_size_must_be_0x1c[
    sizeof(struct SoundPlayerRecord) == 0x1c ? 1 : -1];
#endif

#endif
