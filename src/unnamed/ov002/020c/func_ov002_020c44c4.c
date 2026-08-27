#include "types.h"
// @symbol func_ov002_020c44c4
/* recovered: shared common types, declarations from a shared header */
#include "decl_SaveData.h"
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern u8 data_0209f2d8;
extern int data_0209caa0[];
extern s8 data_0209f2f8;
extern u8 data_0209f264;
extern void _ZN7Vector3D1Ev(void);

extern void func_020731dc(int a, int b, void** node);
extern void Vec3_RotateYAndTranslate(int* out, int* in, short angle, int* src);
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void* player, void* actor, unsigned int id, const void* pos, unsigned int a, unsigned int b);
extern void func_02012694(unsigned int id, const struct Vector3* v);
extern unsigned int func_02012790(unsigned int a);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* thiz, unsigned int a, int b, int fix, unsigned int d);
extern void func_ov002_020c43c4(char* self, unsigned int idx);

int func_ov002_020c44c4(char* self)
{
    struct Vector3 pos;
    int state;
    int r4val;
    u8 f2d8;
    int b0, b1;

    f2d8 = data_0209f2d8;
    r4val = -1;
    b0 = (int)(f2d8 == 1);
    if (b0 != 0) {
        goto cont;
    }
    if ((data_0209caa0[2] & 0x80) == 0) {
        goto ret0;
    }
    b1 = (int)(f2d8 == 2);
    if (b1 == 0) {
        goto cont;
    }
ret0:
    return 0;

cont:
    if (_ZN8SaveData26CountStarsCollectedInLevelEj(SublevelToLevel(data_0209f2f8)) != 0) {
        return 0;
    }

    state = data_0209f2f8;

    switch (state) {
    case 2:
        if ((data_0209caa0[2] & 0x200) == 0) {
            data_0209caa0[2] |= 0x200;
            if ((data_ov002_0210e14c & 1) == 0) {
                data_ov002_0210f3b0.x = 0;
                data_ov002_0210f3b0.y = 0x96000;
                data_ov002_0210f3b0.z = 0x64000;
                func_020731dc((int)&data_ov002_0210f3b0, (int)_ZN7Vector3D1Ev, &data_ov002_0210f350);
                data_ov002_0210e14c |= 1;
            }
            Vec3_RotateYAndTranslate((int*)&pos, (int*)(self + 0x5c), *(s16*)(self + 0x8e), (int*)&data_ov002_0210f3b0);
            _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(self, self, 0x177, &pos, 2, 2);
            func_ov100_02145070(0x1e);
            func_ov100_02145014();
            func_02012694(0xb8, (struct Vector3*)(self + 0x74));
            *(u8*)(self + 0x725) = 1;
            return 1;
        }
        break;
    case 6:
        r4val = 7;
        break;
    case 7:
        r4val = 8;
        break;
    case 10:
        if (data_0209f264 == 0) {
            r4val = 9;
        }
        break;
    case 14:
        if (data_0209f264 == 0) {
            r4val = 10;
        }
        break;
    case 35:
        if (data_0209f264 == 0) {
            r4val = 11;
            func_02012790(0x18);
        }
        break;
    case 31:
        if (data_0209caa0[1] & 0x80000000) {
            return 0;
        }
        r4val = 12;
        break;
    }

    if (r4val == -1) {
        return 0;
    }

    _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x47, 0, 0x1000, 0);
    func_ov002_020c43c4(self, r4val & 0xff);
    return 0;
}
