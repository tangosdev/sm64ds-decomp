//cpp
#include "Ov007SpriteBank.h"
#include "Ov007MenuMem.h"

/* Frees a cell animation's two tables. */
void func_ov007_020c1384(Ov007CellAnim *anim) {
    func_ov007_020c3d1c(anim->frames);
    func_ov007_020c3d1c(anim->durations);
}
