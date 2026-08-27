//cpp
// @symbol func_ov002_020d8944
/* recovered: shared common types */
#include "common.h"
typedef short s16;

// non-trivial destructor keeps the particle-spawn vector on the stack (no SROA)
struct PVec { int x, y, z; ~PVec() {} };

extern "C" {
s16 Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);
int AngleDiff(int a, int b);
void Vec3_Add(struct Vector3* out, struct Vector3* a, struct Vector3* b);
void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern int func_ov002_020d8944(void* self, char* enemy, char* v1);
}

int func_ov002_020d8944(void* self, char* enemy, char* v1){
  PVec p; Vector3 sum; Vector3 asr;
  int b0 = ((unsigned short)*(unsigned short*)(v1 + 0xc) == 0xbf);
  if (b0){
    s16 ang = Vec3_HorzAngle((struct Vector3*)(enemy + 0x5c), (struct Vector3*)(v1 + 0x5c));
    if (AngleDiff(ang, *(s16*)(enemy + 0x8e)) >= 0x4000) return 0;
    *(int*)(enemy + 0x98) = 0xa000;
    *(int*)(v1 + 0x98) = 0xa000;
    *(s16*)(enemy + 0x92) = 0;
    *(s16*)(enemy + 0x94) = ang + 0x8000;
    *(s16*)(enemy + 0x96) = 0;
    *(s16*)(v1 + 0x92) = 0;
    *(s16*)(v1 + 0x94) = ang;
    *(s16*)(v1 + 0x96) = 0;
    Vec3_Add(&sum, (struct Vector3*)(enemy + 0x5c), (struct Vector3*)(v1 + 0x5c));
    Vec3_Asr(&asr, &sum, 1);
    p.x = asr.x; p.y = asr.y; p.z = asr.z;
    p.y += 0x50000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x12a, p.x, p.y, p.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x12b, p.x, p.y, p.z);
    *(unsigned char*)(enemy + 0x726) = 1;
    *(unsigned char*)(v1 + 0x726) = 1;
    return 1;
  }
  return 0;
}
