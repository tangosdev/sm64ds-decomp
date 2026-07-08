struct M48 { int w[12]; };
extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* mF, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* mF, short angX);
extern struct M48 MATRIX_SCRATCH_PAPER;
void func_ov065_02118c4c(char* c) {
  int v[4];
  Vec3_Asr(&v, c+0x5c, 3);
  Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v[0], v[1], v[2]);
  Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(c+0x8e));
  Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(short*)(c+0x8c));
  *(struct M48*)(c+0x10c) = MATRIX_SCRATCH_PAPER;
}
