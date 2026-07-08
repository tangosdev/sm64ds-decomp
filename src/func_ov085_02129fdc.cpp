//cpp
extern "C" {
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void* m, short angZ);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* mtx, int f, int g, unsigned int h);
extern int MATRIX_SCRATCH_PAPER[];

typedef struct { int w[12]; } M48;

void func_ov085_02129fdc(char* c)
{
    Matrix4x3_FromRotationY(c + 0xf0, *(short*)(c + 0x8e));
    *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x118) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;
    *(M48*)MATRIX_SCRATCH_PAPER = *(M48*)(*(char**)(c + 0xe8) + 0x360);
    Matrix4x3_ApplyInPlaceToRotationY(MATRIX_SCRATCH_PAPER, *(short*)(c + 0x362));
    Matrix4x3_ApplyInPlaceToRotationZ(MATRIX_SCRATCH_PAPER, *(short*)(c + 0x360));
    *(M48*)(*(char**)(c + 0xe8) + 0x360) = *(M48*)MATRIX_SCRATCH_PAPER;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x138, c + 0xf0, 0x8c000, 0x32000, 0xf);
}
}
