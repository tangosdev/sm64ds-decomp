
typedef short s16;
typedef long long s64;
extern s16 data_02082214[];
void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src)
{
  int i = (((unsigned short) angle) >> 4) * 2;
  int c = data_02082214[i];
  int s = data_02082214[i + 1];
  int new_var;
  new_var = (int) (((((s64) src[0]) * s) + 0x800) >> 12);
  out[0] = in[0] + (((int) (((((s64) src[2]) * ((unsigned long long) c)) + 0x800) >> 12)) + new_var);
  out[1] = in[1] + src[1];
  out[2] = in[2] + (((int) (((((s64) src[2]) * s) + 0x800) >> 12)) - ((int) (((((s64) src[0]) * c) + 0x800) >> 12)));
}
