#include "types.h"
#include "Ov007Model.h"
#include "math/MtxStack.h"
extern void Matrix4x3_LoadIdentity(void *mat);
extern void func_ov007_020c39f8(void *mat, int a, int b, int c);

/* Builds the model's matrix: rotation, then pos as the translation, then
 * combined with `other` through MulMat4x3Mat4x3. */
void func_ov007_020bc968(Ov007Model *m, void *other) {
    Matrix4x3_LoadIdentity(&m->matrix);
    func_ov007_020c39f8(&m->matrix, m->rotX, m->rotY, m->rotZ);
    m->matrix.t.x = m->pos.x;
    m->matrix.t.y = m->pos.y;
    m->matrix.t.z = m->pos.z;
    MulMat4x3Mat4x3(&m->matrix, other, &m->matrix);
}
