//cpp
// @symbol _ZN12dBgCh_SphCrr14SetFloorResultERK5dBgPi
/* recovered: real C++ method, named members
 *
 * Copies a hit record into the FLOOR slot, mClsnResult1 at 0x074 -- every
 * dBgPi field except its leading 4 bytes of padding, which the ROM leaves
 * alone. Spelled field by field rather than as a whole-struct assignment:
 * the ROM's first move is a 64-bit pair (0x078 <- src+4) and the members
 * reproduce that, where `mClsnResult1 = src_' would also write pad_000.
 */
#include "dBgCh_SphCrr.h"

void dBgCh_SphCrr::SetFloorResult(const dBgPi &src_)
{
    mClsnResult1.unk_004 = src_.unk_004;
    mClsnResult1.unk_00c = src_.unk_00c;
    mClsnResult1.unk_010 = src_.unk_010;
    mClsnResult1.unk_014 = src_.unk_014;
    mClsnResult1.unk_018 = src_.unk_018;
    mClsnResult1.unk_01a = src_.unk_01a;
    mClsnResult1.unk_01c = src_.unk_01c;
    mClsnResult1.unk_020 = src_.unk_020;
    mClsnResult1.unk_024 = src_.unk_024;
}
