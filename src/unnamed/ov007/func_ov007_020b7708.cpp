//cpp
#include "Particle__System.h"

extern "C" {
extern Particle::System *data_ov007_02103450[];
extern void *data_ov007_0210344c;

// The address-named entry point retains its existing caller ABI.
void func_ov007_020b7708(int idx, void *pos) {
    if (data_ov007_02103450[idx] == 0) {
        Particle::System *system;
        data_ov007_02103450[idx] = static_cast<Particle::Manager *>(data_ov007_0210344c)
                                       ->AddSystem(0, *static_cast<Vector3 *>(pos));
        system = data_ov007_02103450[idx];
        system->callbackFlags |= 2;
    }
}
}
