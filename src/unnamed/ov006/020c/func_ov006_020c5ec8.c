typedef struct { int x, y, z; } Vec3;

extern int LenVec3(Vec3* v);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern int NormalizeVec3IfNonZero(Vec3* v);
extern void Vec3_MulScalarInPlace(int* v, int s);
typedef struct { int a, b; } S2;
extern S2 data_ov006_0213af68;

void func_ov006_020c5ec8(char* self)
{
    Vec3 v0, v1, v2, d0, d1, d2, diff;
    int len0, len1, len2;

    *(short*)(self + 0xe6) = 0;
    *(int*)(self + 0xa8) = *(int*)(self + 4);
    *(int*)(self + 0xac) = *(int*)(self + 8);
    *(int*)(self + 0xb0) = *(int*)(self + 0xc);
    *(int*)(self + 0xdc) = LenVec3((Vec3*)(self + 0xa8));

    {
        int y = *(int*)(self + 0x1c);
        int x = *(int*)(self + 0x10);
        v0.x = x;
        v0.y = y;
        v0.z = 0;
    }
    {
        int y = *(int*)(self + 0x1c);
        int x = *(int*)(self + 0x14);
        v1.x = x;
        v1.y = y;
        v1.z = 0;
    }
    {
        int a = *(int*)(self + 0x10);
        int b = *(int*)(self + 0x14);
        int c = *(int*)(self + 0x18);
        v2.x = (a + b) >> 1;
        v2.y = c;
        v2.z = 0;
    }

    Vec3_Sub(&d0, &v0, (Vec3*)(self + 0x9c));
    len0 = LenVec3(&d0);
    Vec3_Sub(&d1, &v1, (Vec3*)(self + 0x9c));
    len1 = LenVec3(&d1);
    Vec3_Sub(&d2, &v2, (Vec3*)(self + 0x9c));
    len2 = LenVec3(&d2);

    if (len2 < len1 && len2 < len0) {
        *(int*)(self + 0xb4) = v2.x;
        *(int*)(self + 0xb8) = v2.y;
        *(int*)(self + 0xbc) = v2.z;
    } else if (len1 < len0) {
        *(int*)(self + 0xb4) = v1.x;
        *(int*)(self + 0xb8) = v1.y;
        *(int*)(self + 0xbc) = v1.z;
    } else {
        *(int*)(self + 0xb4) = v0.x;
        *(int*)(self + 0xb8) = v0.y;
        *(int*)(self + 0xbc) = v0.z;
    }

    Vec3_Sub(&diff, (Vec3*)(self + 0xb4), (Vec3*)(self + 0x9c));
    *(int*)(self + 0xa8) = diff.x;
    *(int*)(self + 0xac) = diff.y;
    *(int*)(self + 0xb0) = diff.z;
    if (NormalizeVec3IfNonZero((Vec3*)(self + 0xa8)) == 0) {
        *(int*)(self + 0xa8) = *(int*)(self + 4);
        *(int*)(self + 0xac) = *(int*)(self + 8);
        *(int*)(self + 0xb0) = *(int*)(self + 0xc);
    } else {
        Vec3_MulScalarInPlace((int*)(self + 0xa8), *(int*)(self + 0xdc));
    }

    *(S2*)(self + 0x30) = data_ov006_0213af68;
}
