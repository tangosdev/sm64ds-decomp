//cpp
// dScStage_c::GraphCallback1 - renders all particles then returns success
#include "dScStage_c.h"

extern "C" void _ZN8Particle9RenderAllEv(void);

int dScStage_c::GraphCallback1() {
    _ZN8Particle9RenderAllEv();
    return 1;
}