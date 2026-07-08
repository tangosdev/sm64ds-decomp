//cpp
typedef short s16;
struct Matrix4x3 { int w[12]; };
struct Mtx43 { int w[12]; };

extern "C" void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3* mF, s16 angX);
extern "C" void Matrix4x3_FromTranslation(Mtx43* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3* mF, s16 angY);

extern Matrix4x3 MATRIX_SCRATCH_PAPER;

struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void m();  // slot 3 -> offset 0xc
};

extern "C" void func_ov006_020c06dc(char* thiz)
{
    char* c = thiz;
    ((Base*)(c + 0x18))->m();
    {
        char* m = *(char**)(c + 0x2c);
        MATRIX_SCRATCH_PAPER = *(Matrix4x3*)(m + 0x120);
    }
    Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(short*)(c + 0xf0));
    {
        char* m = *(char**)(c + 0x2c);
        *(Matrix4x3*)(m + 0x120) = MATRIX_SCRATCH_PAPER;
    }
    Matrix4x3_FromTranslation((Mtx43*)&MATRIX_SCRATCH_PAPER, *(int*)(c + 0xc8), *(int*)(c + 0xcc), *(int*)(c + 0xd0));
    Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(c + 0xea));
    *(Matrix4x3*)(c + 0x34) = MATRIX_SCRATCH_PAPER;
}
