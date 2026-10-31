#include "nitro/fx/fx.h"

typedef int Fix12i;

typedef short s16;

extern void func_0203d480(Fix12i *out, Fix12i *in);
extern s16 data_02082214[];

/* Normalize a Vector2 (Vector2.h): 0 for the zero vector, else 1 after
   func_0203d480 normalizes it. */
int func_0203d434(Fix12i *in) {
    Fix12i out[3];
    if (in[0] == 0 && in[1] == 0)
        return 0;
    func_0203d480(out, in);
    return 1;
}

/* Rotate a Vector2 by angle, the first of Vector2.h's 2-vector helpers. */
void func_0203d388(int *p, int angle) {
    int a = (unsigned short)angle >> 4;
    int c = data_02082214[a * 2];
    int s = data_02082214[a * 2 + 1];
    int x = p[0];
    int z = p[1];
    p[0] = FX_MUL(s, x) - FX_MUL(c, z);
    p[1] = FX_MUL(c, x) + FX_MUL(s, z);
}

/* Vector3's empty constructor. daDpLift_c passes it to __cxa_vec_ctor
   and d_a_ply_methods.cpp to __cxa_vec_new (argument shape: count, size, padding,
   ctor, dtor); see Vector2.h. */
void func_0203d384(void) {}
