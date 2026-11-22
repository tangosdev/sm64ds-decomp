#include "Vector2.h"
#include "Vector2Math.h"
extern "C" {

/* Empty. func_0201a054 calls it during boot, between PXI_Init and
   ClearSaveBlock. */
void func_0203d740(void) {}

/* leftover: Vector3s::Vector3s() and Vector2::Vector2() are already inline.
   An out-of-line definition is a second body, and dropping the inline makes
   every global of those types call a constructor the ROM initializers do not. */
void func_0203d73c(void) {}

/* Vector2's empty constructor, passed to __cxa_vec_ctor for 8-byte
   element arrays (see Vector2.h). */
void func_0203d738(void) {}

/* Add two Vector2s into o (Vector2.h). dScMgHanachan_c chains its body
   segments with it. */
void func_0203d704(Vector2 *o, Vector2 *a, Vector2 *b) {
    int ax = a->x, bx = b->x, ay = a->y, by = b->y;
    o->x = ax + bx;
    o->y = ay + by;
}
}
