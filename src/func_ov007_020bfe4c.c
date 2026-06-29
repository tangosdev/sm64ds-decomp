typedef struct { int x, y, z; } Vector3;
typedef struct { int m[12]; } Matrix4x3;

extern int _ZN4cstd4fdivEii(int a, int b);
extern int _ZN4cstd3divEii(int a, int b);
extern void InvMat4x3(void* dst, void* out);
extern void MulVec3Mat4x3(void* v, void* m, void* res);
extern short data_02082214[];

void func_ov007_020bfe4c(char* thiz, int a, int b, int c, Vector3* out)
{
    Vector3 v;
    Matrix4x3 inv;
    int ang, i, cosv, sinv, ac, f1;
    v.x = a - 0x80000;
    v.y = -(b - 0x60000);
    v.z = c;
    ang = (int)*(unsigned short*)(thiz + 0xd4) >> 4;
    i = ang * 2;
    sinv = data_02082214[i + 1];
    cosv = data_02082214[i];
    ac = c < 0 ? -c : c;
    f1 = _ZN4cstd4fdivEii((int)(((long long)cosv * ac + 0x800) >> 12), sinv);
    v.x = _ZN4cstd3divEii((int)(((long long)v.x * f1 + 0x800) >> 12), 0x60);
    v.y = _ZN4cstd3divEii((int)(((long long)v.y * f1 + 0x800) >> 12), 0x60);
    InvMat4x3(thiz + 0x44, &inv);
    MulVec3Mat4x3(&v, &inv, out);
}
