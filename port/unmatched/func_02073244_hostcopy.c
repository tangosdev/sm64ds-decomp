/* HOST COPY of func_0203cbc0 -- half of the MSL array-delete pair.
 * func_02073244, the other half, is RETIRED (run link100 wave 15, SEAT15F).
 *
 *   func_0203cbc0 STAYS HOSTED. It is a two-instruction veneer onto _ZdlPv and
 *   src still spells it `void func_0203cbc0(void)` calling `_ZdlPv()` with no
 *   argument, which is exactly right for ARM -- the pointer is already in r0
 *   and the branch keeps it there -- and exactly wrong for cdecl, where the
 *   callee would read the stack for an argument nobody declared. Re-read at
 *   8ddff3187: still true, so this one is not a candidate.
 *
 *   func_02073244 IS RETIRED. The copy below it used to say it "hands
 *   func_02073300 THREE arguments and lets the fourth (the element destructor)
 *   ride through in r3", and that "func_02073300 itself is an asm hatch ... so
 *   there is no C to compile for it either". BOTH HALVES ARE STALE at
 *   8ddff3187. src/func_02073244.c now declares
 *   `func_02073300(int a, int b, int c, int dtor)` and passes all four; and
 *   src/func_02073300.cpp exists -- the MSL array-destroy-with-rethrow helper,
 *   recovered with its two nested catch contexts under `#pragma exceptions on`,
 *   which is what the cartridge's .exceptix record at 0x02073994 is for.
 *   Its own callee func_020731fc is src/func_020731fc.c. All three are on
 *   port/slice_l15arm9.txt and the cartridge's text runs the array delete now.
 *
 * The retired host version was the same operation without the register trick:
 * walk the array backwards calling the destructor, then free the raw block.
 * Nothing about the COOKIE LAYOUT changes, and that is worth keeping written
 * down: it is the one hal/actor_vtables.cpp's func_02073470 writes -- the
 * host's own array-new-with-ctor, which mirrors the DS's: the raw block starts
 * with {element size, count} and the array pointer the caller holds is
 * raw + cookie. So the count is at base[-1] for the cookie of 8 that every
 * caller passes, which is what the ROM's own func_02073244 reads
 * (`*(int *)(a - 4)`) and hands to func_02073300 as the element count.
 *
 * Reached from Player::CleanupResources (the 0x32-element queue at +0x578,
 * built by Player::InitResources through the matching func_02073470) and from
 * Stage::CleanupResources, which the port does not run.
 */
extern void _ZdlPv(void *p);

/* THE ONE DS GLOBAL THE RETIREMENT NEEDS. src/func_020731fc.c is
 * `data_0209a6fc()` -- MSL's terminate hook, the thing a second throw out of
 * an array-destroy cleanup lands on. config/arm9/symbols.txt:4485 has it at
 * 0x0209a6fc and the next symbol is _ZTISt9type_info at 0x0209a700, so the ROM
 * span is exactly the four bytes of one function pointer.
 *
 * Hosted here rather than in hal/auto_bss.cpp because the retirement that
 * needs it is this file's, and it is bracketed into .dsstate like every other
 * hosted DS global: dsstate_guard refused the first build of this change for
 * leaving it outside the captured span, and being boot-constant is an argument
 * for the exclusion list that guard keeps by name, not a reason to sit outside
 * it unrecorded. Zero here means a throw escaping a cleanup faults instead of
 * terminating, which is what it did before this file changed too: the whole
 * path was unreachable, because func_02073300 was in no link. */
#include "hal/dsstate_seg.h"
DSSTATE_BEGIN
void (*data_0209a6fc)(void);
DSSTATE_END

/* PORT_HOST_ABI: ARM register ride-through: src spells this
 * `void func_0203cbc0(void)` calling `_ZdlPv()` with no argument, because the
 * pointer is already in r0 and the branch keeps it there. See the header. */
void func_0203cbc0(void *p)
{
    _ZdlPv(p);
}

/* func_02073244's host body was here. See the header: both halves of its
 * reason are stale at 8ddff3187 and the matched TU runs instead. */
