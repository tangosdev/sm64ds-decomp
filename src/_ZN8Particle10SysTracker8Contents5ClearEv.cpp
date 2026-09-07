//cpp
// Readable method name inferred from the registry behavior; not ROM text.
#include "Particle__SysTracker.h"

namespace Particle {

void SysTracker::Contents::Clear()
{
    Entry* entry = mEntries;
    int i;

    for (i = 0; i < 0x40; i++, entry++) {
        if (entry->uniqueID != 0) {
            entry->active = 0;
            Unlink(*entry);
            mCurrentIndex = (u8)i;
        }
    }
}

} // namespace Particle
