#include "Ov007SpriteBank.h"
#include "Ov007MenuMem.h"

/* Frees a sprite-bank cell's OAM attribute list (func_ov007_020c121c's
 * per-cell free). */
void func_ov007_020c13f4(Ov007Cell *cell) {
    func_ov007_020c3d1c(cell->oam);
}
