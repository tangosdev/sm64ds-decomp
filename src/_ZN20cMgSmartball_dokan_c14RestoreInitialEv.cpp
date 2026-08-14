//cpp
/* Slot 2. The ROM is three words -- ldr ip, [pc]; bx ip; .word target -- which
 * is mwcc's interworking-safe TAIL CALL, not a linker veneer and not a
 * "this class does not override" vtable entry. A class that simply inherited
 * the slot would have the base's address sitting in the vtable directly; this
 * one has its own symbol, so the original declared the override and its whole
 * body was the qualified call below. The qualification is load-bearing: an
 * unqualified RestoreInitial() here would dispatch through the vtable and
 * recurse. */
#include "cMgSmartball_dokan_c.h"

void cMgSmartball_dokan_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
}
