typedef short s16;
typedef int s32;
typedef long long s64;

typedef struct {
    s16 x, y, z;
} Vec3s;

extern Vec3s data_02099fc4;
extern void func_02053380(void *a, Vec3s *b);

#define FIX12MUL(a, b) ((s32)(((s64)(a) * (b) + 0x800) >> 12))

void func_0204cebc(void *c)
{
    Vec3s vecA;
    Vec3s vecB;

    vecA = data_02099fc4;
    func_02053380((char *)c + 0x3c, &vecB);

    if (vecA.x == vecB.x && vecA.y == vecB.y && vecA.z == vecB.z) {
        vecA.x = 0x1000;
        vecA.y = 0;
        vecA.z = 0;
    }

    *(s16 *)((char *)c + 0x5c) = (s16)(FIX12MUL(vecB.y, vecA.z) - FIX12MUL(vecB.z, vecA.y));
    *(s16 *)((char *)c + 0x5e) = (s16)(FIX12MUL(vecB.z, vecA.x) - FIX12MUL(vecB.x, vecA.z));
    *(s16 *)((char *)c + 0x60) = (s16)(FIX12MUL(vecB.x, vecA.y) - FIX12MUL(vecB.y, vecA.x));

    *(s16 *)((char *)c + 0x62) = (s16)(FIX12MUL(vecB.y, *(s16 *)((char *)c + 0x60)) - FIX12MUL(vecB.z, *(s16 *)((char *)c + 0x5e)));
    *(s16 *)((char *)c + 0x64) = (s16)(FIX12MUL(vecB.z, *(s16 *)((char *)c + 0x5c)) - FIX12MUL(vecB.x, *(s16 *)((char *)c + 0x60)));
    *(s16 *)((char *)c + 0x66) = (s16)(FIX12MUL(vecB.x, *(s16 *)((char *)c + 0x5e)) - FIX12MUL(vecB.y, *(s16 *)((char *)c + 0x5c)));
}
