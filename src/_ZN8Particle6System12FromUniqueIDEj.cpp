//cpp
// @symbol _ZN8Particle6System12FromUniqueIDEj
#include "Particle__System.h"

namespace Particle {

System *System::FromUniqueID(u32 uniqueID)
{
    SysTracker::Contents::Entry *entry =
        data_0209ee74->mContents.FindData(uniqueID);
    return entry->system;
}

}
