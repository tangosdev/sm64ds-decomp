// @symbol _ZN8dM3dGLin3SetERK7Vector3S2_
/* recovered: named members + shared header */
#include "dM3dGLin.h"
struct V3 { int x, y, z; };
void _ZN8dM3dGLin3SetERK7Vector3S2_(struct dM3dGLin *self, const struct V3* a, const struct V3* b) {
  ((struct V3*)self)->x = a->x;
  ((struct V3*)self)->y = a->y;
  ((struct V3*)self)->z = a->z;
  *(int*)((char*)&self->end + 0) = b->x;
  *(int*)((char*)&self->end + 4) = b->y;
  *(int*)((char*)&self->end + 8) = b->z;
}
