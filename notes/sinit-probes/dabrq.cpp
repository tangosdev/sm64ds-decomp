//cpp
/* Notes-only organic-generation probe for __sinit_ov070_02122d80.
 * It contributes nothing unless SINIT_OWNERSHIP_PROBE is supplied manually.
 * There is deliberately no hand-written initializer in this file. */
#ifdef SINIT_OWNERSHIP_PROBE
#include "types.h"

class BrqModelResourceProbe
{
    u32 words[2];

public:
    BrqModelResourceProbe(u32 fileID);
    ~BrqModelResourceProbe();
};

class BrqAnimationResourceProbe
{
    u32 words[2];

public:
    BrqAnimationResourceProbe(u32 fileID);
    ~BrqAnimationResourceProbe();
};

class BrqTextureResourceProbe
{
    u32 words[2];

public:
    BrqTextureResourceProbe(u32 fileID);
    ~BrqTextureResourceProbe();
};

class BrqVector3Probe
{
    s32 x;
    s32 y;
    s32 z;

public:
    BrqVector3Probe(s32 x_, s32 y_, s32 z_)
        : x(x_), y(y_), z(z_) {}
    ~BrqVector3Probe() {}
};

struct BrqStateProbe
{
    s32 EnterCooldownState();
    s32 UpdateCooldownState();
    s32 EnterActiveState();
    s32 UpdateActiveState();
    s32 EnterDefeatedState();
    s32 UpdateDefeatedState();
};

typedef s32 (BrqStateProbe::*BrqStatePMF)();

struct BrqStatePairProbe
{
    BrqStatePMF enter;
    BrqStatePMF update;
};

BrqModelResourceProbe brqModelResource0(0x2b1);
BrqModelResourceProbe brqModelResource1(0x2b3);
BrqAnimationResourceProbe brqAnimationResource0(0x2b2);
BrqAnimationResourceProbe brqAnimationResource1(0x2b5);
BrqTextureResourceProbe brqTextureResource(0x2b4);

BrqStatePairProbe brqStateHandlers[3] = {
    { &BrqStateProbe::EnterCooldownState,
      &BrqStateProbe::UpdateCooldownState },
    { &BrqStateProbe::EnterActiveState,
      &BrqStateProbe::UpdateActiveState },
    { &BrqStateProbe::EnterDefeatedState,
      &BrqStateProbe::UpdateDefeatedState },
};

BrqVector3Probe brqCylinderOffset(0, -0x28000, 0);
#endif
