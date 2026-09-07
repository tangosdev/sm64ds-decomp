//cpp
// @symbol _ZN10Scuttlebug13OnYoshiTryEatEv
/* daSpd_c::OnYoshiTryEat -- vtable slot 18, byte-matched vs the ROM at mwccarm 2004/b56 with strict relocs (tools/match.py, size from ov071 symbols.txt); the vtable-slot-guess marker it carried was stale.
 * The ROM body ignores `this` and returns a constant. */
#include "Scuttlebug.h"

int Scuttlebug::OnYoshiTryEat()
{
    return 6;
}
