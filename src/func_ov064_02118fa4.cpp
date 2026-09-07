//cpp
extern "C" {
typedef int s32; typedef short s16;
struct Matrix4x3 { s32 a[12]; };
extern Matrix4x3 data_020a0e68;
extern void Matrix4x3_FromTranslation(Matrix4x3*, s32, s32, s32);
void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void* self, const Matrix4x3&, s16);
struct dActor_c { char pad[0x5c]; s32 f5c,f60,f64; };
void func_ov064_02118fa4(dActor_c* c) {
    Matrix4x3_FromTranslation(&data_020a0e68, c->f5c, c->f60 + *(s32*)((char*)c+0x330), c->f64);
    *(Matrix4x3*)((char*)c+0x2ec) = data_020a0e68;
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s((char*)c+0x124, *(Matrix4x3*)((char*)c+0x2ec), *(s16*)((char*)c+0x8e));
}
}
