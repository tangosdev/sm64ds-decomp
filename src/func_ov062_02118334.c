
struct Vec3
{
  int x;
  int y;
  int z;
};
struct Mtx43
{
  int w[12];
};
void Vec3_Asr(struct Vec3 *d, struct Vec3 *s, int sh);
void Matrix4x3_FromTranslation(struct Mtx43 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(struct Mtx43 *m, short angY);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *thiz, void *sm, struct Mtx43 *m, int radHeight, int a, unsigned int b);
extern struct Mtx43 MATRIX_SCRATCH_PAPER;
inline struct Mtx43 *inline_fn()
{
  return &MATRIX_SCRATCH_PAPER;
}

void func_ov062_02118334(char *c)
{
  short *new_var;
  struct Vec3 v;
  Vec3_Asr(&v, (struct Vec3 *) (c + 0x5c), 3);
  if (1)
  {
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    new_var = (short *) (c + 0x8e);
    Matrix4x3_ApplyInPlaceToRotationY(inline_fn(), *new_var);
    *((struct Mtx43 *) (c + 0x31c)) = MATRIX_SCRATCH_PAPER;
    {
      int b = (int) (((*((int *) (c + 0xb0))) & 0x40000) != 0);
      if (b != 0)
      {
        return;
      }
    }
  }
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, (void *) (c + 0x364), (struct Mtx43 *) (c + 0x31c), 0x50000, 0x50000, 0xf);
}
