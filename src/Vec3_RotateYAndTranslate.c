#include "nitro/fx/fx.h"

typedef short s16;
typedef long long s64;
extern s16 data_02082214[];
void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src)
{
  int i = (((unsigned short) angle) >> 4) * 2;
  int c = data_02082214[i];
  int s = data_02082214[i + 1];
  int new_var;
  new_var = FX_MUL(src[0], s);
  out[0] = in[0] + (FX_MUL(src[2], c) + new_var);
  out[1] = in[1] + src[1];
  out[2] = in[2] + (FX_MUL(src[2], s) - FX_MUL(src[0], c));
}
