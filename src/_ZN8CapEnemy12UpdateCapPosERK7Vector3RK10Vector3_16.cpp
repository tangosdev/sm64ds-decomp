//cpp
typedef int Fix12i;
struct Vector3 { Fix12i x, y, z; };
struct Vector3_16 { short x, y, z; };
struct Matrix4x3 { Fix12i a[12]; };
struct CapFlags {
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3to7 : 5;
};

extern "C" void Vec3_Add(Vector3* out, Vector3* a, const Vector3* b);
extern "C" void Vec3_Asr(Vector3* vF, const Vector3* v, int amount);
extern "C" void Matrix4x3_FromTranslation(Matrix4x3* mF, Fix12i x, Fix12i y, Fix12i z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3* mF, short x, short y, short z);
extern Matrix4x3 data_020a0e68;

class CapEnemy {
public:
    void UpdateCapPos(const Vector3& pos, const Vector3_16& rot);
};

void CapEnemy::UpdateCapPos(const Vector3& pos, const Vector3_16& rot)
{
    char* self = (char*)this;
    CapFlags* fl = (CapFlags*)(self + 0x17f);
    if (fl->b2 != 0 || fl->b1 == 0) {
        if ((*(unsigned char*)(self + 0x113) & 0xf) < 6) {
            unsigned char* p = (unsigned char*)(((long long)(int)(self + 0x113)) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p | 0x80;
        }
    } else {
        unsigned char* p = (unsigned char*)(((long long)(int)(self + 0x113)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p & 7;
    }
    if (*(unsigned char*)(self + 0x113) >= 6) return;

    Vector3 sum, asr;
    Vec3_Add(&sum, (Vector3*)(self + 0x5c), &pos);
    Vec3_Asr(&asr, &sum, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, rot.x, rot.y, rot.z);
    *(Matrix4x3*)(self + 0x130) = data_020a0e68;
}
