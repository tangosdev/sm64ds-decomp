typedef unsigned int u32;
typedef short s16;

struct Vec3 { int x, y, z; };
struct M48 { int w[12]; };
struct Flags { unsigned short b0 : 1; };

extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void func_ov002_020e8398(void* self);

void func_ov002_020e84ec(char* self)
{
    struct Vec3 v;
    s16* ang;
    int t;

    if (*(int*)(self + 0x43c) == 8) {
        Vec3_Asr(&v, (struct Vec3*)(self + 0x5c), 3);
        Matrix4x3_FromTranslation(self + 0x328, v.x, v.y, v.z);
    } else if (*(void**)(self + 0xc8) != 0) {
        *(struct M48*)(self + 0x328) = *(struct M48*)(*(char**)(self + 0xc8));
    } else if (!((struct Flags*)(self + 0x4a2))->b0) {
        ang = (s16*)((int)(((long long)(int)(self + 0x8e)) & 0xFFFFFFFFFFFFFFFFLL));
        t = *ang + 0xc00;
        *ang = t;
        Matrix4x3_FromRotationY(self + 0x328, *(s16*)(self + 0x8e));
        *(int*)(self + 0x34c) = *(int*)(self + 0x5c) >> 3;
        *(int*)(self + 0x350) = *(int*)(self + 0x60) >> 3;
        *(int*)(self + 0x354) = *(int*)(self + 0x64) >> 3;
    } else {
        Matrix4x3_FromRotationY(self + 0x328, *(s16*)(self + 0x94));
        *(int*)(self + 0x34c) = *(int*)(self + 0x5c) >> 3;
        *(int*)(self + 0x350) = (*(int*)(self + 0x60) + 0x32000) >> 3;
        *(int*)(self + 0x354) = *(int*)(self + 0x64) >> 3;
    }

    *(struct M48*)(self + 0x38c) = *(struct M48*)(self + 0x328);
    func_ov002_020e8398(self);
}
