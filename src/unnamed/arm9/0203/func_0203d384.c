#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Vector2.h"
#include "Vector2Math.h"

/* Normalize a Vector2 (Vector2.h): 0 for the zero vector, else 1 after
   func_0203d480 normalizes it. */
int func_0203d434(Vector2 *v) {
    Vector2 out;
    if (v->x == 0 && v->y == 0)
        return 0;
    func_0203d480(&out, v);
    return 1;
}

/* Rotate a Vector2 by angle, the first of Vector2.h's 2-vector helpers. */
void func_0203d388(Vector2 *v, s16 angle) {
    int s = FX_SinIdx((unsigned short)angle >> 4);
    int c = FX_CosIdx((unsigned short)angle >> 4);
    int x = v->x;
    int y = v->y;
    v->x = FX_MUL(c, x) - FX_MUL(s, y);
    v->y = FX_MUL(s, x) + FX_MUL(c, y);
}

/* Vector3's empty constructor. daDpLift_c passes it to __cxa_vec_ctor
   and d_a_ply_methods.cpp to __cxa_vec_new (argument shape: count, size, padding,
   ctor, dtor); see Vector2.h. */
void func_0203d384(void) {}
