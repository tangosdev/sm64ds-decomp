/* HOST COPY of src/func_02043288.c -- the actor BEHAVIOR Process with its
 * three member-function pointers resolved to the vtable slots they name.
 * Same treatment, same reason, as port/unmatched/func_0204335c_hostcopy.cpp (the INIT
 * Process): MSVC has no representation for an mwcc pointer-to-member, and
 * the port's copies of the three PMF globals are zeroed host storage.
 *
 * WHICH WRAPPER IS WHICH, read out of the ROM rather than guessed. The four
 * siblings at 0x0204322c / 0x02043288 / 0x020432e4 / 0x0204335c each call
 *
 *     ActorBase::Process(self, <main>, <before>, <after>)
 *
 * with three PMFs out of the twelve at 0x02099e74..0x02099ecc. Those twelve
 * are STATIC data, not runtime-built, and each is the mwcc virtual form
 * { vtable byte offset, 1 }:
 *
 *     0x02099ea4 = { 0x18, 1 }  -> slot  6  Behavior
 *     0x02099eac = { 0x1c, 1 }  -> slot  7  BeforeBehavior
 *     0x02099eb4 = { 0x20, 1 }  -> slot  8  AfterBehavior
 *
 * and func_02043288 is Process(self, 0x02099ea4, 0x02099eac, 0x02099eb4).
 * So this one is the per-frame behaviour tick; 0x0204335c is init (slots
 * 0/1/2), 0x020432e4 cleanup (3/4/5), 0x0204322c render (9/10/11). The same
 * three literals also confirm func_0204335c_hostcopy.cpp's reading of vt[0..2].
 *
 * The control flow is Process's, unchanged (src/_ZN9ActorBase7Process...):
 *
 *     r = Before(); if (r) { r = Main(); code = r==-1 ? 3 : r==1 ? 2 : 1; }
 *                   else   { code = 0; }
 *     After(code);
 *     return r;
 *
 * The slots are __fastcall thunks in the HAL vtables (ecx carries `this`
 * exactly as __thiscall does, the dummy edx absorbs fastcall's second
 * register) -- the calling convention hal/actor_vtables.cpp established.
 */
extern "C" {

typedef int(__fastcall *ActorSlot0)(void *self, void *dummy);
typedef void(__fastcall *ActorSlot2)(void *self, void *dummy, unsigned code);

// PORT_HOST_ABI: ActorBase::Process's three args are mwcc pointer-to-members MSVC cannot represent; resolved to vtable slots 6/7/8 instead
int func_02043288(void *self)
{
    void **vt = *(void ***)self;
    int r = ((ActorSlot0)vt[7])(self, 0);        /* BeforeBehavior */
    unsigned code;
    if (r != 0) {
        r = ((ActorSlot0)vt[6])(self, 0);        /* Behavior */
        code = r == -1 ? 3u : r == 1 ? 2u : 1u;
    } else {
        code = 0;
    }
    ((ActorSlot2)vt[8])(self, 0, code);          /* AfterBehavior */
    return r;
}

}
