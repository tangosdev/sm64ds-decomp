//cpp
typedef int Fix12;
struct Matrix4x3 { int m[12]; };
struct ShadowModel;
struct Actor {
    void DropShadowRadHeight(ShadowModel &sm, Matrix4x3 &mtx, Fix12 a, int b, unsigned int c);
};

extern Matrix4x3 data_02082128;

extern "C" void func_ov062_02116d28(char *c)
{
    *(Matrix4x3 *)(c + 0x390) = data_02082128;
    char *o = *(char **)(c + 0x3f8);
    *(int *)(c + 0x3b4) = *(int *)(o + 0x5c) >> 3;
    o = *(char **)(c + 0x3f8);
    *(int *)(c + 0x3b8) = *(int *)(o + 0x60) >> 3;
    o = *(char **)(c + 0x3f8);
    *(int *)(c + 0x3bc) = *(int *)(o + 0x64) >> 3;
    ((Actor *)c)->DropShadowRadHeight(*(ShadowModel *)(c + 0x368), *(Matrix4x3 *)(c + 0x390), 0x12c000, 0x32000, 0xf);
}
