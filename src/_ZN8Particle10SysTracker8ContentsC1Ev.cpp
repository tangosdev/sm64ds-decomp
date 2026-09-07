//cpp
#include "Particle__SysTracker.h"

Particle::SysTracker::Contents::Contents()
{
    Entry* entry = mEntries;
    int i;

    do {
        entry->uniqueID = 0;
        entry++;
    } while (entry != (Entry*)mBuckets);

    unk_000 = 0;
    mCurrentIndex = 0;
    for (i = 0; i < 0x10; i++)
        mBuckets[i] = 0;
}
