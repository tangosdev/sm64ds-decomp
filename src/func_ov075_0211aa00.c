extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short angY);
extern void InvMat4x3(void* d, void* s);
struct M48 { int w[12]; };
extern struct M48 MATRIX_SCRATCH_PAPER;

void func_ov075_0211aa00(char* r4){
  Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER,
    (*(int*)(r4) - 0xfa000) >> 3,
    (*(int*)(r4 + 4) - 0xfa000) >> 3,
    *(int*)(r4 + 8) >> 3);
  Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(r4 + 0xe));
  *(struct M48*)(r4 + 0x14) = MATRIX_SCRATCH_PAPER;
  InvMat4x3(&MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
  *(struct M48*)(r4 + 0x44) = MATRIX_SCRATCH_PAPER;
}
