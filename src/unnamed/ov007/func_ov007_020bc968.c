#include "types.h"
#include "Ov007Model.h"
#include "math/MtxStack.h"
#define func_ov007_020c39f8 _Z19func_ov007_020c39f8P9Matrix4x3iii
extern void func_ov007_020c39f8(void *mat, int a, int b, int c);

/* Builds the model's matrix: rotation, then pos as the translation, then
 * combined with `other` through MTX_Concat43. */
void func_ov007_020bc968(Ov007Model *m, void *other) {
    MTX_Identity43_(&m->matrix);
    func_ov007_020c39f8(&m->matrix, m->rotX, m->rotY, m->rotZ);
    m->matrix.t.x = m->pos.x;
    m->matrix.t.y = m->pos.y;
    m->matrix.t.z = m->pos.z;
    MTX_Concat43(&m->matrix, other, &m->matrix);
}
