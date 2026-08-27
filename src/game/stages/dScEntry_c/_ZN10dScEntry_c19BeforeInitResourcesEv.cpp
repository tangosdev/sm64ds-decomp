//cpp
// @symbol _ZN10dScEntry_c19BeforeInitResourcesEv
/* recovered: real C++ method */
/* dScEntry_c::BeforeInitResources() -- vtable slot 1. A pure tail call to
 * dScene_c::ResetFadersAndSound: the ROM emits the interworking veneer
 * (ldr ip,[pc]; bx ip; .word 0x0202e66c), not a body. */
#include "dScEntry_c.h"

extern "C" void _ZN8dScene_c19ResetFadersAndSoundEv(void);

/* NO VALUE IS RETURNED, AND THAT IS FAITHFUL. The header declares `bool`; this
 * body sets no return value and the ROM does not either. Adding an explicit
 * `return <v>;` emits a real instruction and breaks the match (measured on
 * PushBlock::OnPushed, 2026-08-22). The legacy .c declared this `void` and never
 * included the header, so nothing checked the disagreement until it became a
 * real method. Harmless for the ROM build; a host-port caller that reads this
 * result gets garbage. Fix upstream in the header's return type if it is wrong --
 * never with a `return` here. */
bool dScEntry_c::BeforeInitResources()
{
    _ZN8dScene_c19ResetFadersAndSoundEv();
}
