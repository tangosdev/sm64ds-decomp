//cpp
// @symbol _ZN6Player17St_EndingFly_MainEv
/* recovered: shared header, real C++ method
 *
 * ov002, 0x020c3d1c, size 0x50. The per-tick handler for the ending flight,
 * dispatching on a phase counter at 0x6e6 to one of three ov002 workers.
 *
 * THE FALLTHROUGH IS DELIBERATE. Phase 1 runs its worker and, when that
 * returns non-zero, falls into phase 2's -- so the frame that finishes phase 1
 * also starts phase 2 rather than idling. Written as a `case 1:` with no
 * `break` on the non-zero path, which is what the ROM's branch structure does.
 *
 * The three workers stay placeholders: they are ov002 functions with no
 * recovered name, and naming them from this call site alone would be a guess.
 *
 * This file carried the placeholder name func_ov002_020c3d1c until the symbol
 * was moved here from ov007, where it had been sitting on an unrelated 0x50
 * function that ov007 calls from 46 places. See the commit and Player.h.
 */
#include "Player.h"

extern "C" {
extern int func_ov002_020c3bdc(char *c);
extern int func_ov002_020c3a48(char *c);
extern int func_ov002_020c38a0(char *c);
}

int Player::St_EndingFly_Main()
{
    switch (mStatePhase) {
    case 0:
        func_ov002_020c3bdc((char *)this);
        break;
    case 1:
        if (func_ov002_020c3a48((char *)this) == 0) break;
    case 2:
        func_ov002_020c38a0((char *)this);
        break;
    }
    return 1;
}
