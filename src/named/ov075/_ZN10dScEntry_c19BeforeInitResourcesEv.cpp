//cpp
// @symbol _ZN10dScEntry_c19BeforeInitResourcesEv
#include "dScEntry_c.h"

/* Slot 1 forwards this and the boolean result through an interworking tail
 * call. ResetFadersAndSound returns only false or true on its two exits. */
bool dScEntry_c::BeforeInitResources()
{
    return ResetFadersAndSound();
}
