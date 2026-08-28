#ifndef PARTICLE__SYSTEM_H
#define PARTICLE__SYSTEM_H

#include "Particle__Manager.h"
#include "Particle__SysTracker.h"

namespace Particle {

struct System {
    struct Flags {
        u32 unk_000 : 6;
        u32 unk_006 : 6;
        u32 unk_00c : 6;
        u32 unused  : 14;
    };

    u8 pad_000[0x18];
    SystemDefinition *mDefinition; /* 0x18 */
    u8 pad_01c[0x58];
    Flags flags;                   /* 0x74 */

    static System *FromUniqueID(u32 uniqueID);
};

typedef char System_size_must_be_0x78[sizeof(System) == 0x78 ? 1 : -1];

}

extern "C" Particle::SysTracker *data_0209ee74;

#endif
