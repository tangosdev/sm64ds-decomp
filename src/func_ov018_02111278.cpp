//cpp
extern "C" {
struct Vector3 { int x,y,z; };
struct Matrix4x3 { int m[12]; };
void Vec3_Asr(struct Vector3* d, struct Vector3* s, int n);
void Matrix4x3_FromTranslation(struct Matrix4x3* mF, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3* mF, short angY);
void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3* mF, short angX);
extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;

void func_ov018_02111278(char* c) {
  struct Vector3 t;
  Vec3_Asr(&t, (struct Vector3*)(c+0x5c), 3);
  Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, t.x, t.y, t.z);
  Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(c+0x8e));
  Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(short*)(c+0x8c));
  Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, 0x4000);
  *(struct Matrix4x3*)(c+0xf0) = MATRIX_SCRATCH_PAPER;
}
}
