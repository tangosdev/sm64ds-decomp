typedef short s16;
typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct Mtx43 { Vec3 r0, r1, r2, t; } Mtx43;

extern void Vec3_Asr(Vec3* d, Vec3* s, int sh);
extern void Vec3_Lsl(Vec3* d, Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(Mtx43* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void* a, void* b, void* c);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* m, int rad, int h, unsigned int u);
extern Mtx43 MATRIX_SCRATCH_PAPER;
extern int data_ov073_021233b0[];
extern int data_ov073_021233e0[];
extern int data_ov073_02123410[];
extern int data_ov073_021233f0[];
extern int data_ov073_02123370[];

void func_ov073_021215cc(char* c)
{
    int sh;
    Vec3 v;
    Vec3 out;
    void* m;
    Mtx43* saved;
    int i;
    Vec3* p;
    char* q;

    Vec3_Asr(&v, (Vec3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER, *(s16*)(c + 0x8c), *(s16*)(c + 0x8e), *(s16*)(c + 0x90));

    p = (Vec3*)(c + 0x4d4);
    *(Mtx43*)(c + 0x328) = MATRIX_SCRATCH_PAPER;
    saved = (Mtx43*)(c + 0x328);
    sh = 3;
    i = 0;
    q = c;
    for (; i < 2; i++) {
        MATRIX_SCRATCH_PAPER = *saved;
        MulMat4x3Mat4x3(*(Mtx43**)(c + 0x320) + (i + 2), &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
        p->x = MATRIX_SCRATCH_PAPER.t.x;
        p->y = MATRIX_SCRATCH_PAPER.t.y;
        p->z = MATRIX_SCRATCH_PAPER.t.z;
        Vec3_Lsl(&out, p, sh);
        *(int*)(q + 0x4d4) = out.x;
        *(int*)(q + 0x4d8) = out.y;
        *(int*)(q + 0x4dc) = out.z;
        p++;
        q += 0xc;
    }

    m = *(void**)(c + 0x37c);
    if (m == (void*)data_ov073_021233b0) return;
    if (m == (void*)data_ov073_021233e0) return;
    if (m == (void*)data_ov073_02123410) return;
    if (m == (void*)data_ov073_021233f0) return;
    if (m == (void*)data_ov073_02123370) return;

    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, *(int*)(c + 0x5c) >> 3, (*(int*)(c + 0x60) - 0xa000) >> 3, *(int*)(c + 0x64) >> 3);
    *(Mtx43*)(c + 0x3a8) = MATRIX_SCRATCH_PAPER;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x380, c + 0x3a8, 0x12c000, 0x3e8000, 0xf);
}
