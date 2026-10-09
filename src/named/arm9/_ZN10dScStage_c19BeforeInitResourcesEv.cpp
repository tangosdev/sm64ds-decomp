//cpp
// @symbol _ZN10dScStage_c19BeforeInitResourcesEv
#include "dScStage_c.h"

/* Slot 1 keeps the fader/sound reset and skips Scene's graphics setup.
 * The boolean result and this pointer pass through the ROM's tail call. */
bool dScStage_c::BeforeInitResources()
{
    return ResetFadersAndSound();
}
