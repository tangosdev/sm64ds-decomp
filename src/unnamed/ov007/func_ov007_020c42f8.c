#include "Ov007Sprite.h"
#include "math/Matrix.h"
#include "nitro/gx/g3imm_cmd.h"

extern void MTX_Identity33_(Matrix3x3 *m);
#define func_ov007_020c3abc _Z19func_ov007_020c3abcP9Matrix3x3iii
extern void func_ov007_020c3abc(Matrix3x3 *out, int ax, int ay, int az);
extern void G3_MultTransMtx33(int *a, int *v);

/* Loads the sprite's transform: translate to pos + posOffset, rotate by
 * rotX/rotY/rotZ, then scale by scaleX/scaleY (z stays 1.0). */
void func_ov007_020c42f8(Ov007Sprite *sprite) {
    int vec[3];
    Matrix3x3 mtx;
    vec[0] = sprite->pos.x + sprite->posOffset.x;
    vec[1] = sprite->pos.y + sprite->posOffset.y;
    vec[2] = sprite->pos.z + sprite->posOffset.z;
    MTX_Identity33_(&mtx);
    func_ov007_020c3abc(&mtx, sprite->rotX, sprite->rotY, sprite->rotZ);
    G3_MultTransMtx33((int *)&mtx, vec);
    G3_Scale(sprite->scaleX, sprite->scaleY, 0x1000);
}
