//cpp
extern "C" {
struct Vec3 { int x, y, z; };
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short angY);
extern void InvMat4x3(void* dst, void* src);
extern int MARIO_CAP_MODEL_PTR[];
extern int LUIGI_CAP_MODEL_PTR[];
extern int WARIO_CAP_MODEL_PTR[];
extern int MATRIX_SCRATCH_PAPER[];

typedef struct { int w[12]; } M48;

int func_ov027_021129dc(char* c)
{
    _ZN5Model8LoadFileER13SharedFilePtr(MARIO_CAP_MODEL_PTR);
    _ZN5Model8LoadFileER13SharedFilePtr(LUIGI_CAP_MODEL_PTR);
    _ZN5Model8LoadFileER13SharedFilePtr(WARIO_CAP_MODEL_PTR);
    *(int*)(c + 0x5c) = 0x3fa770;
    *(int*)(c + 0x60) = 0xcb2000;
    *(int*)(c + 0x64) = 0x46988e;
    *(short*)(c + 0x8e) = 0x5d30;
    Vec3 v;
    Vec3_Asr(&v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationY(MATRIX_SCRATCH_PAPER, *(short*)(c + 0x8e));
    InvMat4x3(MATRIX_SCRATCH_PAPER, MATRIX_SCRATCH_PAPER);
    *(M48*)(c + 0x1394) = *(M48*)MATRIX_SCRATCH_PAPER;
    return 1;
}
}
