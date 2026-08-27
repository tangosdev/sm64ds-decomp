typedef unsigned int u32;
typedef unsigned char u8;
typedef int Fix12i;

typedef struct { int x, y, z; } Vec3;
typedef struct { int x, y, z; } Vector3;

extern void func_ov002_020fea4c(int *a, int *b);
extern void func_ov002_020fea68(int *a, int *b);
extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern void func_020380c0(char *c);

void func_02037608(char *thiz)
{
    Vec3 a;
    Vec3 b;
    *(unsigned char *)(thiz + 0x50) = 0;
    func_ov002_020fea4c((int *)&b, (int *)(thiz + 0x38));
    *(int *)(thiz + 0x54) = b.x;
    *(int *)(thiz + 0x58) = b.y;
    *(int *)(thiz + 0x5c) = b.z;
    func_ov002_020fea68((int *)&a, (int *)(thiz + 0x38));
    *(Fix12i *)(thiz + 0x60) = Vec3_Dist((const Vector3 *)(thiz + 0x54), (const Vector3 *)&a);
    func_020380c0(thiz + 0x10);
}
