//cpp
// @symbol func_ov002_020c8a4c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

struct Player { unsigned int SetAnim(unsigned int, int, int, unsigned int); };
extern "C" short Vec3_HorzAngle(const Vector3*, const Vector3*);
extern "C" void func_020731dc(void*, void*, void*);
extern "C" void Vec3_RotateYAndTranslate(Vector3*, const Vector3*, int, const Vector3*);
extern void func_020072c0(void);

extern "C" int func_ov002_020c8a4c(char* self);
int func_ov002_020c8a4c(char* self) {
    int spd = *(int*)(self + 0x98) >> 3;
    if (spd < 0x400) spd = 0x400;
    ((Player*)self)->SetAnim(0x48, 0, spd, 0);
    *(short*)(self + 0x94) = Vec3_HorzAngle((Vector3*)(self + 0x5c), (Vector3*)(self + 0x744));
    *(short*)(self + 0x8e) = *(short*)(self + 0x94);
    if (func_02053274((Vector3*)(self + 0x5c), (Vector3*)(self + 0x744)) < 0xa000) {
        if (*(unsigned char*)(self + 0x70c) == 0) {
            *(unsigned char*)(self + 0x70c) = 1;
            if (!(data_ov002_0210e150 & 1)) {
                data_ov002_0210f8cc[0] = 0;
                data_ov002_0210f8cc[1] = 0;
                data_ov002_0210f8cc[2] = 0x12c000;
                func_020731dc(data_ov002_0210f8cc, (void*)func_020072c0, &data_ov002_0210f89c);
                data_ov002_0210e150 |= 1;
            }
            Vec3_RotateYAndTranslate((Vector3*)(self + 0x744), (Vector3*)(self + 0x750), *(short*)(self + 0x69e), (Vector3*)data_ov002_0210f8cc);
        } else {
            func_ov002_020ca108(self);
        }
    }
    return 0;
}
