struct Mtx { int w[12]; };
extern struct Mtx MATRIX_SCRATCH_PAPER;
extern void Matrix4x3_FromTranslation(struct Mtx* m, int x, int y, int z);
void func_ov006_020c2290(char* c) {
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, *(int*)(c+0x180), *(int*)(c+0x184), *(int*)(c+0x188));
    *(struct Mtx*)(c+0x24) = MATRIX_SCRATCH_PAPER;
}
