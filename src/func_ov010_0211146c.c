extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short ang);
extern short SINE_TABLE[];
struct M { int w[12]; };
extern struct M MATRIX_SCRATCH_PAPER;
void func_ov010_0211146c(char *c) {
    int a = (int)(*(unsigned short*)(c+0x8e)) >> 4;
    int a2 = a << 1;
    int b = (int)(*(unsigned short*)(c+0x90)) >> 4;
    int cosB = SINE_TABLE[b << 1];
    int sinA = SINE_TABLE[a2 + 1];
    int cosA = SINE_TABLE[a2];
    int r5 = cosB * 5;
    int dx = (int)(((long long)r5 * sinA + 0x800) >> 12);
    int dz = (int)(((long long)r5 * cosA + 0x800) >> 12);
    int X = (*(int*)(c+0x5c) - dx) >> 3;
    int Y = *(int*)(c+0x60) >> 3;
    int Z = (*(int*)(c+0x64) + dz) >> 3;
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, X, Y, Z);
    Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(c+0x8e));
    Matrix4x3_ApplyInPlaceToRotationZ(&MATRIX_SCRATCH_PAPER, *(short*)(c+0x90));
    *(struct M*)(c+0x33c) = MATRIX_SCRATCH_PAPER;
}
