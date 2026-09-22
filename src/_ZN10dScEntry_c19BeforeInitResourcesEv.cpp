//cpp
// @symbol _ZN10dScEntry_c19BeforeInitResourcesEv
/* recovered: real C++ method */
/* dScEntry_c::BeforeInitResources() -- vtable slot 1. A pure tail call to
 * dScene_c::ResetFadersAndSound: the ROM emits the interworking veneer
 * (ldr ip,[pc]; bx ip; .word 0x0202e66c), not a body. */
#include "dScEntry_c.h"

extern "C" bool _ZN8dScene_c19ResetFadersAndSoundEv(void);

/* THIS USED TO RETURN NOTHING, and the note here warned what that would cost:
 * "harmless for the ROM build; a host-port caller that reads this result gets
 * garbage. Fix upstream in the header's return type." That is what happened, one
 * class over. The same shape on Stage::BeforeInitResources skipped the PC port's
 * level boot and faulted on the first frame (run link100, lane CRASH1), so the
 * prediction is now a measurement and the upstream fix is done:
 * dScene_c::ResetFadersAndSound is declared `bool` in include/dScene_c.h, which
 * is what its body has always returned.
 *
 * So the alias above is declared `bool` and the value is returned. The warning
 * about a `return` was about returning a VALUE OF ITS OWN -- writing `return
 * true;` or `return <v>;` here emits a real instruction and breaks the match
 * (measured on daObjPushblock_c::OnPushed, 2026-08-22). Returning the tail
 * call's own result emits nothing, because on ARM the callee's r0 already is
 * this function's r0: byte-verified against ov075 0x0211a734 after the change,
 * still the bare interworking veneer, still 0xc bytes.
 *
 * The arity is unchanged and is still the ROM's: the alias is spelled `(void)`
 * and the receiver rides through r0 (through ecx on the port) exactly as before.
 * Only the width of the answer the caller reads has changed. */
bool dScEntry_c::BeforeInitResources()
{
    return _ZN8dScene_c19ResetFadersAndSoundEv();
}
