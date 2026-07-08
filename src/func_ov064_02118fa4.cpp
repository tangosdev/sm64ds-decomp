//cpp
extern "C" {
typedef int s32; typedef short s16;
struct Matrix4x3 { s32 a[12]; };
extern Matrix4x3 MATRIX_SCRATCH_PAPER;
extern void Matrix4x3_FromTranslation(Matrix4x3*, s32, s32, s32);
void _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(void* self, const Matrix4x3&, s16);
struct Actor { char pad[0x5c]; s32 f5c,f60,f64; };
void func_ov064_02118fa4(Actor* c) {
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, c->f5c, c->f60 + *(s32*)((char*)c+0x330), c->f64);
    *(Matrix4x3*)((char*)c+0x2ec) = MATRIX_SCRATCH_PAPER;
    _ZN18MovingMeshCollider9TransformERK9Matrix4x3s((char*)c+0x124, *(Matrix4x3*)((char*)c+0x2ec), *(s16*)((char*)c+0x8e));
}
}
