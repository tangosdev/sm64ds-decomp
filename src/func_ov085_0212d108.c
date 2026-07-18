typedef short s16;
typedef int s32;

typedef struct { s32 x, y, z; } Vector3;
typedef struct { Vector3 c0, c1, c2, c3; } Matrix4x3;

extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern s16 Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
extern s16 Vec3_VertAngle(const Vector3* v1, const Vector3* v0);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3* mF, s16 angX);
extern void MulVec3Mat4x3(const Vector3* v, const Matrix4x3* m, Vector3* res);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern void func_ov085_0212d268(void* c, void* p);
extern Matrix4x3 data_020a0e68;
extern char data_ov085_0213072c;

int func_ov085_0212d108(char* c)
{
    Vector3 v;
    Vector3 out;
    Vector3 pp;
    char* player;
    s16* ap;
    int zero;
    Vector3* ppos;

    ap = (s16*)(((long long)(int)(c + 0x8c)) & 0xFFFFFFFFFFFFFFFFLL);
    *ap = (s16)(*ap + 0x1000);
    if (*(unsigned short*)(c + 0x100) != 0) {
        return 1;
    }
    player = (char*)_ZN5Actor13ClosestPlayerEv(c);
    if (player == 0) {
        return 1;
    }

    zero = 0;
    ppos = (Vector3*)(((long long)(int)(player + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
    v.x = zero;
    v.y = zero;
    v.z = zero;
    out.x = zero;
    out.y = zero;
    out.z = zero;
    pp.x = ppos->x;
    pp.y = ppos->y;
    pp.z = ppos->z;
    *(s16*)(c + 0x94) = Vec3_HorzAngle((Vector3*)(c + 0x5c), &pp);
    *(s16*)(c + 0x92) = Vec3_VertAngle((Vector3*)(c + 0x5c), &pp);
    v.z = 0x14000;
    Matrix4x3_FromRotationY(&data_020a0e68, *(s16*)(c + 0x94));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16*)(c + 0x92));
    MulVec3Mat4x3(&v, &data_020a0e68, &out);
    _Z14ApproachLinearRiii((int*)(c + 0xa4), out.x, 0x1000);
    _Z14ApproachLinearRiii((int*)(c + 0xa8), out.y, 0x1000);
    _Z14ApproachLinearRiii((int*)(c + 0xac), out.z, 0x1000);
    if (pp.y + 0x64000 > *(int*)(c + 0x60)) {
        *(void**)(c + 0x18c) = player;
        func_ov085_0212d268(c, &data_ov085_0213072c);
    }
    return 1;
}
