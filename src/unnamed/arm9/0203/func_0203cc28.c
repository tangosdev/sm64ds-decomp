#include "types.h"
#include "math/CalcLib.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"

/* Rotate a vector about the X axis by angle, with the table sine and cosine
   at angle >> 4: y and z turn, x is kept. The
   trampoline Mario (d_mg_trmpln2_mario.cpp) tilts its vectors with it. */
void func_0203cd80(Vector3 *v, short angle) {
    unsigned int i = (unsigned short)angle >> 4;
    short s = FX_SinIdx(i);
    short c = FX_CosIdx(i);
    int z = v->z;
    int y = v->y;
    int ny = FX_MUL(c, y) - FX_MUL(s, z);
    int nz = FX_MUL(s, y) + FX_MUL(c, z);
    v->y = ny;
    v->z = nz;
}

/* The same about the Y axis: x and z turn, y is kept. */
void func_0203ccd4(Vector3 *v, short angle) {
    unsigned int i = (unsigned short)angle >> 4;
    short s = FX_SinIdx(i);
    short c = FX_CosIdx(i);
    int x = v->x;
    int z = v->z;
    int nx = FX_MUL(c, x) + FX_MUL(s, z);
    int nz = FX_MUL(c, z) - FX_MUL(s, x);
    v->x = nx;
    v->z = nz;
}

/* The same about the Z axis: x and y turn, z is kept. Vector2.h's
   func_0203d388 has the same body over a Vector2. The trampoline minigame
   turns its vectors with it. */
void func_0203cc28(Vector3 *v, int angle) {
    int i = (unsigned short)angle >> 4;
    int s = FX_SinIdx(i);
    int c = FX_CosIdx(i);
    int x = v->x;
    int y = v->y;
    v->x = FX_MUL(c, x) - FX_MUL(s, y);
    v->y = FX_MUL(s, x) + FX_MUL(c, y);
}
