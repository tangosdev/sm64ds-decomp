typedef struct { int x, y, z; } Vec3;
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int Vec3_HorzLen(Vec3* v);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angX);
extern void MulVec3Mat4x3(Vec3* v, void* m, Vec3* dst);
extern void Vec3_Add(Vec3* out, Vec3* a, Vec3* b);
extern void Vec3_MulScalar(Vec3* out, Vec3* v, int s);
extern int data_020a0e68[];

void func_ov014_0211236c(char* c)
{
    Vec3 tmp;
    Vec3 head;
    Vec3 rotated;
    Vec3 sum;
    Vec3 prevpos;
    Vec3 diff;
    Vec3 delta;
    Vec3 scaled;
    Vec3 mul;
    short angY;
    Vec3* target;
    Vec3* cur;
    short angX;
    int i;
    int z;

    z = *(int*)(c + 0x5f8);
    head.x = 0;
    head.y = 0;
    head.z = z;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;

    prevpos.x = *(int*)(c + 0x5ec);
    {
        int py = *(int*)(c + 0x5f0);
        prevpos.y = py;
        prevpos.z = *(int*)(c + 0x5f4);
        prevpos.y = py + 0x1e000;
    }

    z = *(int*)(c + 0x5f8);
    head.x = 0;
    head.y = 0;
    head.z = z;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;

    cur = (Vec3*)(c + 0x56c);
    i = 6;
    do {
        target = (i == 6) ? &prevpos : (Vec3*)((char*)cur + 0xc);
        Vec3_Sub(&diff, cur, target);
        tmp.x = diff.x;
        tmp.y = diff.y;
        tmp.z = diff.z;
        angY = _ZN4cstd5atan2E5Fix12IiES1_(tmp.x, tmp.z);
        angX = (short)(-_ZN4cstd5atan2E5Fix12IiES1_(tmp.y, Vec3_HorzLen(&tmp)));
        Matrix4x3_FromRotationY(data_020a0e68, angY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, angX);
        MulVec3Mat4x3(&head, data_020a0e68, &rotated);
        sum.x = cur->x;
        sum.y = cur->y;
        sum.z = cur->z;
        Vec3_Add(&delta, target, &rotated);
        cur->x = delta.x;
        cur->y = delta.y;
        cur->z = delta.z;
        Vec3_Sub(&scaled, cur, &sum);
        Vec3_MulScalar(&mul, &scaled, 0xb68);
        sum.x = mul.x;
        sum.y = mul.y;
        sum.z = mul.z;
        i = i - 1;
        cur = (Vec3*)((char*)cur - 0xc);
    } while (i >= 0);
}
