//cpp
#include "Ov007Sprite.h"

extern "C" {

void func_ov007_020bfe4c(Ov007Camera *camera, int x, int y, int z, Vector3 *out);

/* Places a sprite in the world: unprojects its screen position at its depth
 * through the camera into pos. */
void func_ov007_020c4388(Ov007Sprite *sprite, Ov007Camera *camera) {
    func_ov007_020bfe4c(camera, sprite->screenX, sprite->screenY, sprite->depth, &sprite->pos);
}
}
