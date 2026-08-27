#include "types.h"
// DeathTable_ClearBit: clears the bit in the actor death table for the given deathTableID
extern s8 data_0209f2f8;
extern u32 data_0209f4f8[];

extern s32 GetLevelPart(s32 level);

void DeathTable_ClearBit(s32 id) {
    if (id < 0) return;
    s32 level = GetLevelPart((s32)data_0209f2f8);
    u32* table = data_0209f4f8 + (level << 4);
    u32 bitMask = ~(1u << (id & 0x1f));
    s32 word = id >> 5;
    table[word] = table[word] & bitMask;
}
