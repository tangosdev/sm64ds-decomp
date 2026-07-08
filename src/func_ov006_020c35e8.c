typedef struct { int x, y, z; } Vec3;
typedef struct { int x, y, z, w; } Quat;

extern int _Z15ApproachLinear2Rsss(short* p, short a, short b);
extern void AddVec3(void* a, void* b, void* c);
extern void Vec3_MulScalarInPlace(int* v, int s);
extern void Vec3_Add(void* out, void* a, void* b);
extern int DotVec3(void* a, void* b);
extern void Vec3_MulScalar(void* out, void* in, int scale);
extern void SubVec3(void* a, void* b, void* c);
extern int NormalizeVec3IfNonZero(void* v);
extern void Quaternion_FromVector3(void* out, void* a, void* b);
extern void Quaternion_SLerp(void* q0, void* q1, int t, void* out);
extern void Quaternion_Normalize(void* q);
extern void func_0202ffec(void* a, void* b, void* c);

extern Quat data_02092768;
extern int data_02082214[];

void func_ov006_020c35e8(char* self)
{
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
    Vec3 v3;
    Quat q0;
    Quat q1;
    int idx;
    int val;
    int dot;

    if (_Z15ApproachLinear2Rsss((short*)(self + 0x94), 0, 1) != 0) {
        *(int*)(self + 0x40) = 0;
        return;
    }
    AddVec3(self, self + 0xc, self);
    *(int*)(self + 0x1c) = -0x40;
    AddVec3(self + 0xc, self + 0x18, self + 0xc);
    idx = ((*(int*)(self + 4) >> 1) << 16) >> 20;
    val = *(short*)&data_02082214[idx];
    v0.x = val;
    v0.y = 0;
    v0.z = val;
    Vec3_MulScalarInPlace((int*)&v0, 0x400);
    Vec3_Add(&v1, self + 0xc, &v0);
    dot = DotVec3(self + 0x24, &v1);
    Vec3_MulScalar(&v2, self + 0x24, dot);
    Vec3_MulScalarInPlace((int*)&v2, 0x300);
    SubVec3(self + 0xc, &v2, self + 0xc);
    v3.x = v1.x;
    v3.y = v1.y;
    v3.z = v1.z;
    if (NormalizeVec3IfNonZero(&v3) == 0) return;
    q1 = data_02092768;
    Quaternion_FromVector3(&q0, self + 0x24, &v3);
    Quaternion_SLerp(&q1, &q0, 0x200, &q1);
    Quaternion_Normalize(&q1);
    func_0202ffec(self + 0x30, &q1, self + 0x30);
}
