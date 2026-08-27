//cpp
typedef unsigned short u16;
typedef short s16;
extern "C" {
extern s16 data_02082214[];
void func_02052550(int* m, int a, int b);
void func_0205256c(int* m, int a, int b);
void Matrix3x3_SetRotationZ(int* m, int a, int b);
void MulVec3Mat3x3(const int *in, const int *m, int *out);
}
struct EffectData { u16 angle; u16 mode; };
struct Vector3 { int x, y, z; };

extern "C" void _ZN8Particle4Turn4FuncERNS_10EffectDataEPcR7Vector3(EffectData& e, char* p, Vector3& v)
{
    int mtx[10];
    int idx;
    switch (e.mode) {
    case 0:
        idx = e.angle >> 4;
        func_02052550(mtx, data_02082214[idx*2], data_02082214[idx*2+1]);
        break;
    case 1:
        idx = e.angle >> 4;
        func_0205256c(mtx, data_02082214[idx*2], data_02082214[idx*2+1]);
        break;
    case 2:
        idx = e.angle >> 4;
        Matrix3x3_SetRotationZ(mtx, data_02082214[idx*2], data_02082214[idx*2+1]);
        break;
    }
    MulVec3Mat3x3((int*)(p + 0x14), mtx, (int*)(p + 0x14));
}
