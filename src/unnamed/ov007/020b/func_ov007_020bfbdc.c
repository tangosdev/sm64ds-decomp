typedef struct { int x, y, z; } Vec3;
typedef struct { int m[12]; } Mtx43;
extern void *data_ov007_02104bd8;
extern void func_ov007_020bc47c(void *thiz);
extern int _ZN4cstd3divEii(int a, int b);
extern void InvMat4x3(void *src, Mtx43 *dst);
extern void MulVec3Mat4x3(Vec3 *in, Mtx43 *mat, void *out);

void func_ov007_020bfbdc(void *self)
{
    char *s = (char*)self;
    char *o = (char*)((void**)data_ov007_02104bd8)[1];
    Vec3 vec;
    Mtx43 mat;

    func_ov007_020bc47c(o);
    *(short*)(o + 0x4c) = (short)_ZN4cstd3divEii(0x5a0000, 0x168);
    *(int*)(o + 0x94) = 0;

    vec.y = 0;
    vec.x = 0;
    vec.z = 0xffffef9c;
    InvMat4x3(s + 0x44, &mat);
    MulVec3Mat4x3(&vec, &mat, o + 0x34);
}
