#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"

typedef short s16;

extern void func_0203d480(fx32 *out, fx32 *in);

/* Normalize a Vector2 (Vector2.h): 0 for the zero vector, else 1 after
   func_0203d480 normalizes it. */
int func_0203d434(fx32 *in) {
    fx32 out[3];
    if (in[0] == 0 && in[1] == 0)
        return 0;
    func_0203d480(out, in);
    return 1;
}

/* Rotate a Vector2 by angle, the first of Vector2.h's 2-vector helpers. */
void func_0203d388(int *p, int angle) {
    int s = FX_SinIdx((unsigned short)angle >> 4);
    int c = FX_CosIdx((unsigned short)angle >> 4);
    int x = p[0];
    int z = p[1];
    p[0] = FX_MUL(c, x) - FX_MUL(s, z);
    p[1] = FX_MUL(s, x) + FX_MUL(c, z);
}

/* Vector3's empty constructor. daDpLift_c passes it to __cxa_vec_ctor
   and d_a_ply_methods.cpp to __cxa_vec_new (argument shape: count, size, padding,
   ctor, dtor); see Vector2.h. */
void func_0203d384(void) {}
