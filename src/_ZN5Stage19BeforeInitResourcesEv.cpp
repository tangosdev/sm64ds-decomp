//cpp
/* Stage::BeforeInitResources() at 0x0202ddc8, 0xc bytes -- vtable slot 1.
 *
 * A tail call to Scene::ResetFadersAndSound (0x0202e66c), emitted as
 * `ldr ip,[pc]; bx ip; .word` because the build is -interworking; see
 * src/_ZN5Scene11AfterRenderEj.cpp for why that shape says nothing about branch
 * range.
 *
 * Note what it forwards to: not Scene's BeforeInitResources, but the non-virtual
 * helper that one calls first. Stage keeps the fader and sound reset and skips
 * the 3D-graphics reinitialisation its base would also do.
 *
 * THE MISSING `return` IS DELIBERATE AND IS THE HONEST SPELLING HERE. Read
 * before ' fixing' it:
 *
 *   - The return type must be `bool`, because ActorBase declares slot 1 that way
 *     and an override whose return type differs is a nineteenth slot, not an
 *     override.
 *   - Scene::ResetFadersAndSound is declared `int`.
 *   - So `return ResetFadersAndSound();` makes the compiler insert an int->bool
 *     normalisation, which turns the three-word tail call into a real call with
 *     a prologue. Measured: both that and `return ... != 0;` diverge from the
 *     ROM. Only the bare call reproduces it.
 *
 * That is not a trick -- it is what a tail call means. The callee's r0 becomes
 * this function's r0 untouched, and no conversion happens anywhere in the ROM
 * either. It is runbook section 8's point that a veneer's return type is
 * unobservable at its own definition, in the one shape where the two ends
 * genuinely disagree.
 *
 * The clean fix is elsewhere: declare Scene::ResetFadersAndSound `bool`, which
 * is what its body actually returns (mov r0,#1 / moveq r0,#0), and then
 * `return ResetFadersAndSound();` is both correct and a tail call. That touches
 * a different class, so it is not in this PR.
 */
#include "Stage.h"

bool Stage::BeforeInitResources()
{
    ResetFadersAndSound();
}
