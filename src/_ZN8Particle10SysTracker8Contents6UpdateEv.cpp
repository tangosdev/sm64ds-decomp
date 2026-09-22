//cpp
// Readable method name inferred from the registry behavior; not ROM text.
#include "Particle__System.h"

namespace Particle {

void SysTracker::Contents::Update()
{
    Entry* entry = mEntries;
    int i;

    for (i = 0; i < 0x40; i++, entry++) {
        if (entry->uniqueID == 0)
            continue;

        if (entry->callback != 0) {
            bool callbackActive = entry->active == 1;
            if (entry->callback->OnUpdate(*entry->system, callbackActive))
                entry->active = 1;
        }

        {
            int isActive = (int)(entry->active == 1);
            if (isActive != 0) {
                entry->active = 0;
                continue;
            }
        }

        Unlink(*entry);
        mCurrentIndex = (u8)i;
    }
}

} // namespace Particle
