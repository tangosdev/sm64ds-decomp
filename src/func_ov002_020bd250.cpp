//cpp
// @symbol func_ov002_020bd250
/* recovered: shared common types */
#include "common.h"

struct Player {
    void Hurt(const Vector3&, unsigned int, int, unsigned int, unsigned int, unsigned int);
};
namespace Particle { struct System { static void NewSimple(unsigned int, int, int, int); }; }
extern "C" short ReadUnalignedShort(unsigned char* p);
extern "C" void Vec3_RotateYAndTranslate(Vector3* out, const Vector3* base, int angle, const Vector3* off);

extern "C" int func_ov002_020bd250(char* self, unsigned char* p);
int func_ov002_020bd250(char* self, unsigned char* p) {
    Vector3 out1, off1, out2, off2, v;
    *(short*)(self + 0x8e) = ReadUnalignedShort(p);
    off1.x = 0;
    off1.y = 0;
    off1.z = 0x50000;
    Vec3_RotateYAndTranslate(&out1, (Vector3*)(self + 0x5c), *(short*)(self + 0x8e), &off1);
    *(unsigned char*)(self + 0x6de) = 1;
    *(unsigned char*)(self + 0x6df) = 0;
    v.x = out1.x;
    v.y = out1.y;
    v.z = out1.z;
    ((Player*)self)->Hurt(v, 0, 0xc000, 1, 0, 1);
    *(int*)(self + 0xa8) = 0x20000;
    *(int*)(self + 0x98) = 0xc000;
    if (*(int*)(self + 8) == 1) {
        off2.x = 0;
        off2.y = 0x64000;
        off2.z = 0x32000;
        Vec3_RotateYAndTranslate(&out2, (Vector3*)(self + 0x5c), *(short*)(self + 0x8e), &off2);
        Particle::System::NewSimple(0x43, out2.x, out2.y, out2.z);
        Particle::System::NewSimple(0x44, out2.x, out2.y, out2.z);
    }
    return 1;
}
