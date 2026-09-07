//cpp
// Readable method name inferred from the registry behavior; not ROM text.
#include "Particle__SysTracker.h"

namespace Particle {

void SysTracker::Contents::Unlink(Entry& entry)
{
    Entry* prev = entry.prev;
    Entry* next = entry.next;

    if (prev == 0)
        mBuckets[entry.uniqueID & 0xf] = next;
    else
        prev->next = next;

    if (next != 0)
        next->prev = prev;

    entry.Reset();
}

} // namespace Particle
