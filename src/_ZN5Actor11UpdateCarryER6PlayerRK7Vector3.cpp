//cpp
typedef int Fix12i;

struct Vector3 { Fix12i x, y, z; };
struct Matrix4x3 { Fix12i a[12]; };

struct Player;

extern "C" void *func_ov002_020e496c(Player *p);
extern "C" void MulMat4x3Mat4x3(Matrix4x3 *m1, Matrix4x3 *m0, Matrix4x3 *mF);
extern "C" void Matrix4x3_FromTranslation(Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *mF, short x, short y, short z);
extern "C" void Vec3_Asr(Vector3 *vF, const Vector3 *v, int amount);
extern "C" void Vec3_Lsl(Vector3 *vF, const Vector3 *v, int amount);
extern "C" void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);

extern Matrix4x3 data_020a0e68;

class Actor {
public:
    Matrix4x3 *UpdateCarry(Player &player, const Vector3 &vec);
};

Matrix4x3 *Actor::UpdateCarry(Player &player, const Vector3 &vec)
{
    Player *p = &player;
    char *self = (char *)this;
    Vector3 t;
    *(int *)((char *)&t + 0) = 0;
    *(int *)((char *)&t + 4) = 0;
    *(int *)((char *)&t + 8) = 0;
    void *o = func_ov002_020e496c(p);
    char *base = *(char**)((char*)o + 0x14);
    data_020a0e68 = *(Matrix4x3*)((char*)o + 0x1c);
    MulMat4x3Mat4x3((Matrix4x3*)(base + 0x2a0), &data_020a0e68, &data_020a0e68);
    *(Vector3 *)&t = *(Vector3 *)((char *)&data_020a0e68 + 0x24);
    *(short*)(self + 0x94) = *(short*)((char*)p + 0x8e);
    *(short*)(self + 0x8e) = *(short*)(self + 0x94);
    Matrix4x3_FromTranslation(&data_020a0e68,
        *(volatile int *)((char *)&t + 0),
        *(volatile int *)((char *)&t + 4),
        *(volatile int *)((char *)&t + 8));
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(short*)(self + 0x8c), *(short*)(self + 0x8e), *(short*)(self + 0x90));
    Vector3 asr;
    Vec3_Asr(&asr, &vec, 3);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    *(Fix12i*)(self + 0x5c) = data_020a0e68.a[9];
    *(Fix12i*)(self + 0x60) = data_020a0e68.a[10];
    *(Fix12i*)(self + 0x64) = data_020a0e68.a[11];
    volatile Vector3 lsl;
    Vec3_Lsl((Vector3*)&lsl, (Vector3*)(self + 0x5c), 3);
    *(Fix12i*)(self + 0x5c) = lsl.x;
    *(Fix12i*)(self + 0x60) = lsl.y;
    *(Fix12i*)(self + 0x64) = lsl.z;
    return &data_020a0e68;
}
