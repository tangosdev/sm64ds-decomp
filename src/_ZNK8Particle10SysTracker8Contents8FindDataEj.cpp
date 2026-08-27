//cpp
// @symbol _ZNK8Particle10SysTracker8Contents8FindDataEj
/* recovered: real class form -- the compiler spells the mangled name */
#include "Particle__SysTracker.h"

namespace Particle {

void *SysTracker::Contents::FindData(u32 uniqueID) const
{
    if (uniqueID == 0)
        return 0;

    char *node = (char *)mBuckets[uniqueID & 0xf];
    while (node) {
        if ((s32)uniqueID == *(s32 *)node)
            return node;
        node = *(char **)(node + 0x18);
    }
    return 0;
}

}
