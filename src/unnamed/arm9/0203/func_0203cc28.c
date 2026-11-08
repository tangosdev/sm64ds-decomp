#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"

/* Rotate a vector about the X axis by angle, with the table sine and cosine
   at angle >> 4: y and z turn, x is kept. The
   trampoline Mario (d_mg_trmpln2_mario.cpp) tilts its vectors with it. */
void func_0203cd80(int *m, short angle) {
    unsigned int i = (unsigned short)angle >> 4;
    short s = FX_SinIdx(i);
    short c = FX_CosIdx(i);
    int z = m[2];
    int y = m[1];
    int ny = FX_MUL(c, y) - FX_MUL(s, z);
    int nz = FX_MUL(s, y) + FX_MUL(c, z);
    m[1] = ny;
    m[2] = nz;
}

/* The same about the Y axis: x and z turn, y is kept. */
void func_0203ccd4(int *m, short angle) {
    unsigned int i = (unsigned short)angle >> 4;
    short s = FX_SinIdx(i);
    short c = FX_CosIdx(i);
    int x = m[0];
    int z = m[2];
    int nx = FX_MUL(c, x) + FX_MUL(s, z);
    int nz = FX_MUL(c, z) - FX_MUL(s, x);
    m[0] = nx;
    m[2] = nz;
}

/* Rotate a Vector2 {x, y} by angle, with the same table. Vector2.h's
   func_0203d388 has the same body. The trampoline minigame turns its
   vectors with it. */
void func_0203cc28(int *p, int angle) {
    int i = (unsigned short)angle >> 4;
    int s = FX_SinIdx(i);
    int c = FX_CosIdx(i);
    int x = p[0];
    int y = p[1];
    p[0] = FX_MUL(c, x) - FX_MUL(s, y);
    p[1] = FX_MUL(s, x) + FX_MUL(c, y);
}
