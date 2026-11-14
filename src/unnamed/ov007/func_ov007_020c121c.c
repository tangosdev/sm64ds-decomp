#include "Ov007SpriteBank.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c13f4(Ov007Cell *cell);
#define func_ov007_020c1384 _Z19func_ov007_020c1384P13Ov007CellAnim
extern void func_ov007_020c1384(Ov007CellAnim *anim);

/* Frees a sprite bank: each cell's OAM list (func_ov007_020c13f4), each
 * animation's tables (func_ov007_020c1384), both tables and the bank. */
void func_ov007_020c121c(Ov007SpriteBank *bank) {
    int i;
    int j;
    for (i = 0; i < bank->cellCount; i++) {
        func_ov007_020c13f4(&bank->cells[i]);
    }
    func_ov007_020c3d1c(bank->cells);
    for (j = 0; j < bank->animCount; j++) {
        func_ov007_020c1384(&bank->anims[j]);
    }
    func_ov007_020c3d1c(bank->anims);
    func_ov007_020c3d1c(bank);
}
