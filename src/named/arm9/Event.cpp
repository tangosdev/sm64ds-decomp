//cpp
/* Event.cpp -- arm9 event-flag bitfield accessors: Clear/Set/Get over the
 * one-word event table data_0209f34c. Covers 0x02029ea4..0x02029ef8, the span
 * between DeathTable_GetBit above and Stage::RenderVsModeNewStar below.
 *
 * `#pragma defer_codegen off` makes mwccarm emit .text in source order, which
 * is the ROM's order here. Do not reorder.
 */
#include "Event.h"

#pragma defer_codegen off

namespace Event {

// @symbol _ZN5Event8ClearBitEj
s32 ClearBit(u32 bit)
{
    return data_0209f34c &= ~(1 << bit);
}

// @symbol _ZN5Event6SetBitEj
void SetBit(u32 bit)
{
    data_0209f34c |= 1 << bit;
}

// @symbol _ZN5Event6GetBitEj
s32 GetBit(u32 bit)
{
    return data_0209f34c & (1 << bit);
}

}
