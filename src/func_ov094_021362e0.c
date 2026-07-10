typedef int Fix12i;
typedef short s16;

struct Vec3 { Fix12i x, y, z; };
struct Mtx43 { Fix12i a[12]; };

extern struct Mtx43 data_020a0e68;
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Mtx43* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(struct Mtx43* m, int x, int y, int z);

void func_ov094_021362e0(char* c)
{
    struct Vec3 v;
    char* m;
    int z = 0;
    if (*(int*)(c + 0x3cc) == 0) return;
    *(volatile Fix12i*)&v.x = z;
    *(volatile Fix12i*)&v.y = z;
    *(volatile Fix12i*)&v.z = z;
    m = *(char**)(c + 0x3cc);
    data_020a0e68 = *(struct Mtx43*)(*(char**)(m + 0xc8));
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x3000, z, z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, -0x6000, 0x1000, 0x4000);
    v.x = data_020a0e68.a[9];
    v.y = data_020a0e68.a[10];
    v.z = data_020a0e68.a[11];
    *(int*)(c + 0x5c) = v.x << 3;
    *(int*)(c + 0x60) = v.y << 3;
    *(int*)(c + 0x64) = v.z << 3;
    *(s16*)(c + 0x94) = *(s16*)(*(char**)(c + 0x3cc) + 0x8e);
    *(s16*)(c + 0x92) = (s16)z;
    *(struct Mtx43*)(c + 0x328) = data_020a0e68;
}
