//cpp
// @symbol _ZNK8Particle10SysTracker8Contents8FindDataEj
/* recovered: real class form -- the compiler spells the mangled name */
#include "Particle__SysTracker.h"

namespace Particle {

SysTracker::Contents::Entry *SysTracker::Contents::FindData(u32 uniqueID) const
{
    if (uniqueID == 0)
        return 0;

    Entry *entry = mBuckets[uniqueID & 0xf];
    while (entry) {
        if (uniqueID == entry->uniqueID)
            return entry;
        entry = entry->next;
    }
    return 0;
}

}
