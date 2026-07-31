#include "types.h"
// DeathTable_SetBit: sets the bit in the actor death table for the given deathTableID
extern s8 LEVEL_ID;
extern u32 ACTOR_DEATH_TABLE_ARR[];

extern s32 GetLevelPart(s32 level);

void DeathTable_SetBit(s32 id) {
    if (id < 0) return;
    s32 level = GetLevelPart((s32)LEVEL_ID);
    u32* table = ACTOR_DEATH_TABLE_ARR + (level << 4);
    s32 word = id >> 5;
    u32 val = table[word];
    u32 bitMask = 1u << (id & 0x1f);
    table[word] = val | bitMask;
}
