/* Animation::Advance, host copy with the ARM divider's semantics -- lane
 * CAPSHOW's substitution for src/_ZN9Animation7AdvanceEv.cpp in slice_gate7
 * and slice_gate10 (the CMake loops skip the src line; the slice files are
 * untouched).
 *
 * WHY: the looping arm computes `(currFrame + speed + len) % len`, and len is
 * ZERO on a ModelAnim that carries a model and no animation. That is the VS
 * cap: WaterfallMist::InitResources seats only the BMD for mType 4
 * (ModelBase::SetFile at +0x300, no Animation::LoadFile, no SetAnim), sets
 * speed to 0x1000 (unk_35c), and WaterfallMist::Behavior calls
 * Animation::Advance(this + 0x350) every frame it is not hidden. Before the
 * cap-visibility manager ran, every VS cap was hidden and Behavior returned
 * ahead of the call; seat the manager (slice_cap.txt) and the first showing
 * cap divides by zero on its first frame (measured: c0000094 in
 * Animation::Advance +0x1e, walker actor id 0x10d, VS map 2 level 43).
 *
 * WHAT THE ROM DOES: mwccarm compiles that `%` as `bl 0x01ffabe4`
 * (config/arm9/relocs.txt: from 0x02015c64 arm_call to 0x01ffabe4, itcm),
 * the ITCM signed divide that returns the quotient in r0 and the remainder in
 * r1. Its prologue is `cmp r1,#0 ; beq epilogue` with r0 = |n| and r1 = 0, and
 * the epilogue only re-applies the signs, so on the DS n / 0 = n and
 * n % 0 = 0. Advance therefore parks currFrame at 0 and carries on; nothing
 * reads the frame because ModelAnim's file pointer (+0x360) is null and the
 * pose is never rebuilt. x86 idiv faults instead. This copy changes the one
 * modulus and nothing else. PORT_HOST_ABI: ARM AEABI division semantics
 * (n % 0 = 0). Note the port's own __aeabi_idiv host body
 * (hal/actor_vtables.cpp) answers n / 0 = 0 where the ROM answers n; that is
 * a quotient site and not this one, recorded in status/CAPSHOW.md.
 */
// @symbol _ZN9Animation7AdvanceEv
#include "Animation.h"
extern "C" int __aeabi_idivmod(int n, int d);

/* the ARM divider's remainder: n % 0 answers 0 (see the banner) */
static inline int arm_srem(int n, int d) { return d ? n % d : 0; }

void Animation::Advance()
{
    u32 f = numFramesAndFlags;
    u32 len = f & ~0xc0000000;
    if ((f & 0xc0000000) == 0) {
        currFrame = arm_srem(currFrame + speed + (int)len, (int)len);
    } else {
        /* the ROM materializes the field address for this
           read-modify-write, and reloads currFrame through the member
           afterwards; the launder keeps this store aliasing the member so
           the compiler re-reads it the way the ROM does */
        int *pFrame = (int *)((long long)((int)this + 8));
        *pFrame = *pFrame + speed;
        if (currFrame < 0) {
            currFrame = 0;
        } else if (currFrame >= (int)len) {
            currFrame = (int)len - 1;
        }
    }
}
