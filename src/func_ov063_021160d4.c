
typedef struct
{
  int m[12];
} Matrix4x3;
typedef struct
{
  int x;
  int y;
  int z;
} Vec3;
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
extern void MulMat4x3Mat4x3(Matrix4x3 *dst, Matrix4x3 *a, Matrix4x3 *b);
extern void SubVec3(Vec3 *dst, Vec3 *a, Vec3 *b);
extern void Vec3_MulScalarInPlace(Vec3 *v, int s);
extern Matrix4x3 data_020a0e68;
inline int inline_fn(int arg0)
{
  return data_020a0e68.m[arg0];
}

void func_ov063_021160d4(char *c)
{
  int *p;
  *((int *) (c + 0x534)) = 0;
  *((int *) (c + 0x538)) = 0;
  *((int *) (c + 0x53c)) = 0;
  Matrix4x3_FromTranslation(&data_020a0e68, *((int *) (c + 0x5c)), *((int *) (c + 0x60)), *((int *) (c + 0x64)));
  MulMat4x3Mat4x3(*((Matrix4x3 **) (c + 0x394)), &data_020a0e68, &data_020a0e68);
  *((int *) (c + 0x534)) = data_020a0e68.m[9];
  *((int *) (c + 0x538)) = inline_fn(10);
  *((int *) (c + 0x53c)) = inline_fn(11);
  SubVec3((Vec3 *) (c + 0x534), (Vec3 *) (c + 0x5c), (Vec3 *) (c + 0x534));
  SubVec3((Vec3 *) (c + 0x534), (Vec3 *) (c + 0x540), (Vec3 *) (c + 0x534));
  Vec3_MulScalarInPlace((Vec3 *) (c + 0x534), 0x6800);
  p = (int *) (((int) c + 0x53c) & 0xFFFFFFFFFFFFFFFF);
  *p += *((int *) (c + 0x598));
}
