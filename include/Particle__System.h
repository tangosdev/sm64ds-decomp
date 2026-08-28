#ifndef PARTICLE__SYSTEM_H
#define PARTICLE__SYSTEM_H

#include "Particle__SysTracker.h"

namespace Particle {

struct System {
    static System *FromUniqueID(u32 uniqueID);
};

}

extern "C" Particle::SysTracker *data_0209ee74;

#endif
