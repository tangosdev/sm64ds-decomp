//cpp
extern "C" {
struct Matrix4x3 { int m[12]; };
extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;
void Matrix4x3_FromTranslation(struct Matrix4x3 *mF, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *mF, short angY);
void func_ov006_020cafdc(char *o) {
  Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, *(int*)(o+0x1c), *(int*)(o+0x20), *(int*)(o+0x24));
  Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(o+0x4a));
  *(struct Matrix4x3*)(o+0x88) = MATRIX_SCRATCH_PAPER;
}
}
