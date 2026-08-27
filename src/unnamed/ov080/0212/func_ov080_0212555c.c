typedef int Fix12i;
typedef struct { int x, y, z; } Vec3;
typedef struct { int m[12]; } Matrix4x3;

extern Matrix4x3 data_020a0e68;

extern void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
extern void InvMat4x3(Matrix4x3 *dst, Matrix4x3 *inv);

void func_ov080_0212555c(char *self)
{
    Vec3 v;
    Vec3_Asr(&v, (Vec3*)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *(short*)(self + 0x8c), *(short*)(self + 0x8e), *(short*)(self + 0x90));
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, -(((unsigned char)(*(int*)(self + 8) & 0xf) + 1) * 0x64000) / 2 >> 3, 0, 0);
    *(Matrix4x3*)(self + 0xd4) = data_020a0e68;
    InvMat4x3(&data_020a0e68, &data_020a0e68);
    *(Matrix4x3*)(self + 0x104) = data_020a0e68;
}
