//cpp
// Stage::GraphCallback1 - renders all particles then returns success
#include "Stage.h"

extern "C" void _ZN8Particle9RenderAllEv(void);

int Stage::GraphCallback1() {
    _ZN8Particle9RenderAllEv();
    return 1;
}