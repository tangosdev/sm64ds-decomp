#ifndef PORT_HAL_PORT_D16_H
#define PORT_HAL_PORT_D16_H

/* SLOT 16 IS THE ONE VTABLE SLOT MSVC DISPATCHES ITSELF, AND ITS CONTRACT IS
 * NOT THE VTABLE LAW'S.
 *
 * The vtable law (hal/actor_classes.cpp) says every entry is a two-argument
 * __fastcall thunk: `this` in ecx, a dummy edx absorbing fastcall's second
 * register, nothing on the stack. That law was written for the four Process
 * wrappers in port/unmatched/, which dispatch slots 0..12 through raw
 * function-pointer casts of exactly that shape, so it holds for every slot
 * those wrappers reach.
 *
 * Slot 16 is not reached by any of them. Its one and only dispatcher in the
 * whole image is MATCHED SOURCE:
 *
 *     src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp   ...   this->~fBase_c();
 *
 * and MSVC compiles a virtual destructor call through its own single
 * destructor slot (the scalar deleting destructor, ??_G), which is __thiscall
 * with ONE CALLEE-POPPED STACK ARGUMENT, the delete flag:
 *
 *     005f5347  mov  eax, [esi]           ; the vtable
 *     005f5349  mov  ecx, esi             ; this
 *     005f534b  push 0                    ; the flag -- the CALLEE must pop it
 *     005f534d  call dword ptr [eax+0x40] ; slot 16
 *     005f5350  push ...                  ; no `add esp,4` anywhere after it
 *
 * A two-argument __fastcall face ends in a bare `ret` and pops nothing, so the
 * flag is left on the stack and AfterCleanupResources' own epilogue unwinds
 * four bytes low: `pop esi` takes the flag, `pop ebp` takes the saved esi, and
 * `ret 4` jumps to the SAVED EBP -- a stack address, which Windows kills the
 * process for executing. Measured on the shipped artifact, esp identical
 * across the call at 005f534d and 005f5350.
 *
 * The faces are port scaffolding and the dispatcher is matched decomp source,
 * so the adaptation belongs here, at the store, not in the matched TU and not
 * in the 264 faces. PORT_D16 wraps a face in the shape MSVC's destructor slot
 * actually has. The wrapper takes the flag as its one stack argument and pops
 * it; the face keeps the vtable law's two-argument shape, unchanged, and is
 * still called the way every other slot calls it.
 *
 * The ROM ignores the flag too: slot 16 is the Itanium D1, the complete-object
 * destructor, and the deallocation is done by the caller
 * (fBase_c::AfterCleanupResources ends in fBase_c::operator delete). A flag of
 * 1 would mean "also free me", which nothing on this path ever passes, so the
 * wrapper drops it rather than acting on it -- and slot 17, the D0 that would
 * free, stays trapped exactly as before.
 *
 * Same contract shape as port_actor_s30_base, which the vtable law already
 * describes as "this in ecx, the hidden result pointer the one (callee-popped)
 * stack argument".
 */
/* Run link100, lane DTORCALL: count the dispatches. Between 9cbd99048 and that
 * lane there were NONE AT ALL -- the two-arm destructor spelling made
 * AfterCleanupResources' `this->~fBase_c()` a direct call to fBase_c's own
 * body, so these wrappers existed and were never entered, and nothing reported
 * it. The counter is one increment on a teardown path and prints nothing
 * unless SM64DS_DTORCALL_CENSUS is set. It does not touch this function's
 * calling convention: the `ret 4` is still the wrapper's own. */
#include "dtorcall_census.h"

template <auto Face>
static int __fastcall port_d16_face(void *self, void *dummy, unsigned /*flag*/)
{
    port_dtorcall_note(self);
    return (int)(size_t)Face(self, dummy);
}

#define PORT_D16(f) (&port_d16_face<f>)

#endif
