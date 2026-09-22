//cpp
/* Stage::BeforeInitResources() at 0x0202ddc8, 0xc bytes -- vtable slot 1.
 *
 * A tail call to dScene_c::ResetFadersAndSound (0x0202e66c), emitted as
 * `ldr ip,[pc]; bx ip; .word` because the build is -interworking; see
 * src/_ZN8dScene_c11AfterRenderEj.cpp for why that shape says nothing about branch
 * range.
 *
 * Note what it forwards to: not dScene_c's BeforeInitResources, but the non-virtual
 * helper that one calls first. Stage keeps the fader and sound reset and skips
 * the 3D-graphics reinitialisation its base would also do.
 *
 * THIS FILE USED TO HAVE NO `return`, AND SAID SO ON PURPOSE. That is settled
 * now, so here is the whole history in one place.
 *
 * The return type has to be `bool`, because fBase_c declares slot 1 that way and
 * an override whose return type differs is a nineteenth slot, not an override.
 * dScene_c::ResetFadersAndSound was declared `int`. So `return
 * ResetFadersAndSound();` made the compiler insert an int->bool normalisation,
 * which turned the three-word tail call into a real call with a prologue;
 * `return ... != 0;` did the same. Only a bare call with no `return` reproduced
 * the ROM, and the note here argued that was honest, because a tail call means
 * the callee's r0 becomes this function's r0 untouched and the ROM converts
 * nothing either.
 *
 * It was honest about the cartridge and wrong about everything else. The PC port
 * compiles this same source with a host compiler that makes no such promise: a
 * function that falls off its end returns whatever happens to be in the return
 * register, and there slot 1 handed back a value that measured as 0. The ROM's
 * own init Process (func_0204335c) calls slot 1 FIRST and, when it answers 0,
 * runs slot 2 and returns without ever calling slot 0 -- the level boot. So the
 * boot silently never ran, port_stage_a_boot returned null, and the port faulted
 * on the first frame at port/tests/walk_window.cpp:8335. Run link100, lane CRASH1
 * measured the whole chain; this is the fix its evidence named.
 *
 * The fix is the one this note already pointed at: dScene_c::ResetFadersAndSound
 * is declared `bool` now, which is what its body has always returned (mov r0,#1 /
 * moveq r0,#0), so `return ResetFadersAndSound();` is bool-to-bool, needs no
 * conversion, and stays the same three-word interworking tail call. Byte-verified
 * against the ROM after the change: this function, ResetFadersAndSound itself and
 * dScene_c::BeforeInitResources all still reproduce exactly.
 *
 * The general point stands and is worth keeping: a veneer's return type is
 * unobservable at its own definition on ARM (runbook section 8). The lesson is
 * that "unobservable in the ROM" is not the same as "unobservable", and where the
 * two ends of a tail call disagree, the answer is to make them agree rather than
 * to leave a function without a `return`.
 */
#include "Stage.h"

bool Stage::BeforeInitResources()
{
    return ResetFadersAndSound();
}
