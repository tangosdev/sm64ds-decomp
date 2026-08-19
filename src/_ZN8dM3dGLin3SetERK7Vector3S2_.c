// @symbol _ZN11RaycastLine4Line3SetERK7Vector3S3_
/* recovered: named members + shared header */
#include "RaycastLine__Line.h"
struct V3 { int x, y, z; };
void _ZN11RaycastLine4Line3SetERK7Vector3S3_(struct RaycastLine__Line *self, const struct V3* a, const struct V3* b) {
  ((struct V3*)self)->x = a->x;
  ((struct V3*)self)->y = a->y;
  ((struct V3*)self)->z = a->z;
  *(int*)((char*)&self->unk_00c) = b->x;
  *(int*)((char*)&self->unk_010) = b->y;
  *(int*)((char*)&self->unk_014) = b->z;
}
