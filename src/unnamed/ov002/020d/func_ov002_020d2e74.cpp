//cpp
#include "types.h"
// @symbol func_ov002_020d2e74
/* recovered: shared common types */
#include "common.h"
struct Player {
    char pad8[8];
    int field_8;             /* 0x8 */
    char pad1[0x5c - 0xc];
    struct Vector3 field_5c; /* 0x5c */
    char pad2[0x74 - 0x68];
    struct Vector3 field_74; /* 0x74 */
    char pad3[0x8e - 0x80];
    s16 field_8e;            /* 0x8e */
    char pad4[0x6d9 - 0x90];
    u8 field_6d9;            /* 0x6d9 */
    char pad5[0x707 - 0x6da];
    u8 field_707;            /* 0x707 */
};

extern "C" void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, const struct Vector3 &v);
extern "C" void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, const struct Vector3 &v);
extern "C" void Vec3_RotateYAndTranslate(struct Vector3 *out, const struct Vector3 *base, s16 ang, const struct Vector3 *off);
extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 a, int b, int c, int d);

extern "C" void func_ov002_020d2e74(struct Player *thiz)
{
    struct Vector3 v;
    struct Vector3 off;
    if (thiz->field_707 != 0)
        return;
    if (thiz->field_8 != 3)
        return;
    _ZN5Sound9PlayBank0EjRK7Vector3(0xbc, thiz->field_74);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(thiz->field_6d9, 0x2f, thiz->field_74);
    off.x = 0;
    off.y = 0x1e000;
    off.z = 0x64000;
    Vec3_RotateYAndTranslate(&v, &thiz->field_5c, thiz->field_8e, &off);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xfe, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xff, v.x, v.y, v.z);
}
