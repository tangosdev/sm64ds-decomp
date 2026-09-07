//cpp
// Readable method name inferred from the registry behavior; not ROM text.
#include "Particle__SysTracker.h"

namespace Particle {

void SysTracker::Contents::Link(Entry& entry)
{
    int index = entry.uniqueID & 0xf;
    Entry* head = mBuckets[index];

    if (head == 0) {
        mBuckets[index] = &entry;
    } else {
        head->prev = &entry;
        entry.next = mBuckets[index];
        mBuckets[index] = &entry;
    }
}

} // namespace Particle
