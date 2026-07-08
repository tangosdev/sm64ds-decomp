//cpp
typedef unsigned int u32;
typedef short s16;

typedef struct Mtx43 { int w[12]; } Mtx43;

extern "C" void Matrix4x3_FromRotationY(void* m, int angle);
extern "C" void Matrix4x3_ApplyInPlaceToTranslation(void* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void* m, s16 angX);
extern "C" void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* c, void* shadow, void* mtx, int rad, int height, u32 flags);

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

extern "C" void func_ov071_0211f524(char* c)
{
    int b = (int)((*(u32*)(c + 0xb0) & 0x40000) != 0);
    if (b) {
        if (*(int*)(c + 0x39c) == 0)
            return;
    }

    Matrix4x3_FromRotationY(c + 0xf0, *(s16*)(c + 0x8e));
    *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x118) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;

    if (*(s16*)(c + 0x8c) != 0) {
        MATRIX_SCRATCH_PAPER = *(Mtx43*)(c + 0xf0);
        int y1 = ((VObj*)c)->m29() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(s16*)(c + 0x8c));
        int y2 = (-((VObj*)c)->m29()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, y2, 0);
        *(Mtx43*)(c + 0xf0) = MATRIX_SCRATCH_PAPER;
    }

    *(int*)(c + 0x374) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x378) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x37c) = *(int*)(c + 0x64) >> 3;

    int dh = (*(int*)(c + 0x39c) == 8) ? 0x190000 : 0xc8000;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x138, c + 0x350, 0xa0000, dh, 0xf);
}
