typedef int s32;
typedef struct { s32 x, y, z; } Vector3;
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angX);
extern void MulVec3Mat4x3(const Vector3* v, const void* m, Vector3* res);
extern int MATRIX_SCRATCH_PAPER[];

void func_ov062_0211b2fc(char* c){
  Vector3 v[2];
  v[0].z = 0;
  v[0].x = 0;
  v[0].y = 0;
  v[0].z = 0;
  v[1].x = 0;
  v[1].y = 0;
  v[1].z = 0;
  v[0].x = 0;
  v[0].y = 0;
  v[0].z = 0x14000;
  if (*(int*)(c+0x468) == 0 && *(int*)(c+0x44c) == 0) {
    if (*(unsigned short*)(c+0x444) == 0) {
      short* a = (short*)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFF);
      *a = *a - 0x100;
    }
    v[0].z = 0x28000;
  }
  Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER, *(short*)(c+0x8e));
  Matrix4x3_ApplyInPlaceToRotationX(MATRIX_SCRATCH_PAPER, *(short*)(c+0x8c));
  MulVec3Mat4x3(&v[0], MATRIX_SCRATCH_PAPER, (Vector3*)(c+0xa4));
}
