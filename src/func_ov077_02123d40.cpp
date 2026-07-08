//cpp
typedef unsigned int u32;
typedef unsigned char u8;
typedef short s16;

typedef struct Mtx43 { int w[12]; } Mtx43;

extern "C" void Matrix4x3_FromRotationY(void* m, int angle);
extern "C" void Matrix4x3_ApplyInPlaceToTranslation(void* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void* m, s16 angX);
extern "C" void Matrix4x3_ApplyInPlaceToRotationY(void* m, s16 angY);
extern "C" void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* c, void* shadow, void* mtx, int rad, int height, u32 flags);
extern "C" void MulMat4x3Mat4x3(void* a, void* b, void* out);
extern "C" void Vec3_LslInPlace(void* v, int sh);
extern "C" void Vec3_Asr(void* d, void* s, int sh);
extern "C" void Matrix4x3_FromTranslation(void* m, int x, int y, int z);

extern Mtx43 MATRIX_SCRATCH_PAPER;

struct VObj {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual int  m29();  // slot 0x74 = index 29
};

extern "C" void func_ov077_02123d40(char* c)
{
    int b = (int)((*(u32*)(c + 0xb0) & 0x40000) != 0);
    if (b)
        return;

    Matrix4x3_FromRotationY(c + 0xf0, *(s16*)(c + 0x8e));
    *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x118) = (*(int*)(c + 0x60) + *(int*)(c + 0x414)) >> 3;
    *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;

    if (*(int*)(c + 0x3f4) == 4) {
        MATRIX_SCRATCH_PAPER = *(Mtx43*)(c + 0xf0);
        int y1 = ((VObj*)c)->m29() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(s16*)(c + 0x8c));
        int y2 = (-((VObj*)c)->m29()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, y2, 0);
        *(Mtx43*)(c + 0xf0) = MATRIX_SCRATCH_PAPER;
    }

    *(int*)(c + 0x3e4) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3e8) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x3ec) = *(int*)(c + 0x64) >> 3;

    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x188, c + 0x3c0, 0x5a000, 0x320000, 0xf);

    if (*(int*)(c + 0x3f4) != 1)
        return;

    u32 t = (u32)(*(int*)(c + 0x12c) << 4) >> 0x10;
    if (t < 0x19)
        return;
    if (t > 0x3a)
        return;

    struct { int t[3]; int v[3]; } lv;
    lv.t[0] = -0x2000;
    lv.t[1] = 0x1800;
    lv.t[2] = 0x1000;

    MATRIX_SCRATCH_PAPER = *(Mtx43*)(c + 0xf0);
    MulMat4x3Mat4x3(*(char**)(c + 0xe8) + 0x90, &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);

    *(int*)(c + 0x404) = MATRIX_SCRATCH_PAPER.w[9];
    *(int*)(c + 0x408) = MATRIX_SCRATCH_PAPER.w[10];
    *(int*)(c + 0x40c) = MATRIX_SCRATCH_PAPER.w[11];

    Vec3_LslInPlace(c + 0x404, 3);

    Vec3_Asr(lv.v, c + 0x404, 3);

    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, lv.v[0], lv.v[1], lv.v[2]);
    Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(s16*)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, lv.t[0], lv.t[1], lv.t[2]);

    *(Mtx43*)(c + 0x154) = MATRIX_SCRATCH_PAPER;
}
