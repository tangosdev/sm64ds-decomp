#include "Ov007SpriteBank.h"
#include "nitro/mi/memory.h"
#include "Ov007MenuMem.h"

/* Allocates a sprite bank for `cellCount` cells and `animCount`
 * animations, each table zeroed record by record. */
Ov007SpriteBank *func_ov007_020c12ac(int cellCount, int animCount) {
    Ov007SpriteBank *bank = func_ov007_020c3df4(0, sizeof(Ov007SpriteBank));
    bank->cells = func_ov007_020c3df4(0, cellCount * sizeof(Ov007Cell));
    bank->anims = func_ov007_020c3df4(0, animCount * sizeof(Ov007CellAnim));
    bank->cellCount = cellCount;
    bank->animCount = animCount;
    {
        volatile int v1;
        int i;
        for (i = 0; i < cellCount; i++) {
            v1 = 0;
            MI_CpuFill32((int *)&bank->cells[i], v1, sizeof(Ov007Cell));
        }
    }
    {
        volatile int v2;
        int j;
        for (j = 0; j < animCount; j++) {
            v2 = 0;
            MI_CpuFill32((int *)&bank->anims[j], v2, sizeof(Ov007CellAnim));
        }
    }
    return bank;
}
