struct Mtx { int w[12]; };
extern struct Mtx MATRIX_SCRATCH_PAPER;
extern void Matrix4x3_FromTranslation(struct Mtx* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(struct Mtx* m, short angY);
void func_ov006_020c4c00(char* c) {
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, *(int*)(c+0x9c), *(int*)(c+0xa0), *(int*)(c+0xa4));
    Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(c+0xe6));
    *(struct Mtx*)(c+0x54) = MATRIX_SCRATCH_PAPER;
}
