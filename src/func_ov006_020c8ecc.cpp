//cpp
extern "C" {
struct Matrix4x3 { int m[12]; };
extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;
void Matrix4x3_FromTranslation(struct Matrix4x3 *mF, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *mF, short angY);
void func_ov006_020c8ecc(char *o) {
  Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, *(int*)(o+0x24), *(int*)(o+0x28), *(int*)(o+0x2c));
  Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(o+0x52));
  *(struct Matrix4x3*)(o+0x94) = MATRIX_SCRATCH_PAPER;
}
}
