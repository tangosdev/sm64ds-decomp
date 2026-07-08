//cpp
extern "C" void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short angZ);
struct M48 { int w[12]; };
extern M48 MATRIX_SCRATCH_PAPER;
struct C {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5(void *arg);
};
extern "C" void func_ov006_02122814(char *c);
void func_ov006_02122814(char *c) {
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    Matrix4x3_ApplyInPlaceToRotationZ(&MATRIX_SCRATCH_PAPER, *(short *)(c + 0x74));
    *(M48 *)(c + 0x1c) = MATRIX_SCRATCH_PAPER;
    ((C *)c)->v5(c + 0x50);
}
