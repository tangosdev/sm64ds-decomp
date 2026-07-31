#include "types.h"
// DeathTable_GetBit: gets the bit from the actor death table for the given deathTableID
extern s8 LEVEL_ID;
extern u32 ACTOR_DEATH_TABLE_ARR[];

extern s32 GetLevelPart(s32 level);

u32 DeathTable_GetBit(s32 id) {
    if (id < 0) return 0;
    s32 level = GetLevelPart((s32)LEVEL_ID);
    u32* table = ACTOR_DEATH_TABLE_ARR + (level << 4);
    s32 word = id >> 5;
    s32 bit = id & 0x1f;
    u32 bitMask = 1u << bit;
    return table[word] & bitMask;
}
