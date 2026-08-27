//cpp
struct M48 { int w[12]; };
struct Vec3 { int x, y, z; };

extern "C" {
extern struct M48 data_020a0e68;
extern void Matrix4x3_ApplyInPlaceToTranslation(struct M48 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(struct M48 *m, int x, int y, int z);
extern void Matrix4x3_FromTranslation(struct M48 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(struct M48 *m, int x, int y, int z);

void func_ov002_020ed998(char *c)
{
    int on = (*(int *)(c + 0xb0) & 0x100) != 0;
    if (on && *(int *)(c + 0x38c)) {
        volatile struct Vec3 v;
        int t9, t10, t11;
        v.x = 0;
        v.y = 0;
        v.z = 0;
        data_020a0e68 = *(struct M48 *)(*(char **)(*(char **)(c + 0x38c) + 0xc8));
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x3f, 9, 0xb);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, 0xffffb60b, 0xfffff3e9, 0xffffd6c1);
        t9  = ((volatile struct M48 *)&data_020a0e68)->w[9];
        t10 = ((volatile struct M48 *)&data_020a0e68)->w[10];
        t11 = ((volatile struct M48 *)&data_020a0e68)->w[11];
        v.y = t10;
        v.z = t11;
        v.x = t9;
        *(int *)(c + 0x5c) = t9 << 3;
        *(int *)(c + 0x60) = v.y << 3;
        *(int *)(c + 0x64) = v.z << 3;
        return;
    }
    Matrix4x3_FromTranslation(&data_020a0e68,
        (int)((((long long)*(int *)(c + 0x5c) << 9) + 0x800) >> 12),
        (int)((((long long)(*(int *)(c + 0x60) + 0x14000) << 9) + 0x800) >> 12),
        (int)((((long long)*(int *)(c + 0x64) << 9) + 0x800) >> 12));
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(short *)(c + 0x8c), *(short *)(c + 0x8e), *(short *)(c + 0x90));
    *(struct M48 *)(c + 0x31c) = data_020a0e68;
}
}
