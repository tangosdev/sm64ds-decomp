//cpp
// Readable method name inferred from the registry behavior; not ROM text.
#include "Particle__SysTracker.h"

namespace Particle {

u32 SysTracker::Contents::Create(
    u32 definitionID, Vector3& position, const Vector3_16f *direction,
    dPa_c::level_c::callback_c *callback)
{
    int i;

    for (i = 0; i < 0x40; i++) {
        if (mEntries[mCurrentIndex].uniqueID == 0) {
            ++unk_000;
            if (unk_000 == 0)
                ++unk_000;

            if (!mEntries[mCurrentIndex].Initialise(
                    unk_000, definitionID, position, direction, callback))
                return 0;

            Link(mEntries[mCurrentIndex]);
            return unk_000;
        }

        mCurrentIndex = (mCurrentIndex + 1) % 0x40;
    }

    return 0;
}

} // namespace Particle
