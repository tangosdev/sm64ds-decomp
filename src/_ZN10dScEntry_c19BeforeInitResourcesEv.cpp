//cpp
// @symbol _ZN10dScEntry_c19BeforeInitResourcesEv
/* recovered: real C++ method */
/* dScEntry_c::BeforeInitResources() -- vtable slot 1. A pure tail call to
 * dScene_c::ResetFadersAndSound: the ROM emits the interworking veneer
 * (ldr ip,[pc]; bx ip; .word 0x0202e66c), not a body. */
#include "dScEntry_c.h"

extern "C" void _ZN8dScene_c19ResetFadersAndSoundEv(void);

bool dScEntry_c::BeforeInitResources()
{
    _ZN8dScene_c19ResetFadersAndSoundEv();
}
