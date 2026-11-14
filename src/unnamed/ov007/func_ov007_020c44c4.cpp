//cpp
#include "Ov007Sprite.h"
#include "Ov007MenuMem.h"


/* Frees a sprite func_ov007_020c44e4 built: its frames array, then the
 * sprite. */
void func_ov007_020c44c4(Ov007Sprite *sprite) {
    func_ov007_020c3d1c(sprite->frames);
    func_ov007_020c3d1c(sprite);
}
