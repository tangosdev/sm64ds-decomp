// @symbol func_ov006_020c6088
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern void _Z14ApproachLinearRsss(short *a, short b, short c);

void func_ov006_020c6088(char *c)
{
    int base_y = *(int*)(c + 0x24);
    int pos_y = *(int*)(c + 0xa0);
    if (pos_y < base_y) {
        int vel = *(int*)(c + 0xac);
        if (vel < 0) {
            *(int*)(c + 0xac) = -vel;
            *(short*)(c + 0xea) = 0;
            goto after_y;
        }
    }
    if (pos_y > base_y + 0x60000) {
        int vel = *(int*)(c + 0xac);
        if (vel > 0) {
            *(int*)(c + 0xac) = -vel;
            *(short*)(c + 0xea) = 0;
        }
    }
after_y:
    {
    int base_x = *(int*)(c + 0x20);
    int pos_x = *(int*)(c + 0x9c);
    if (pos_x < base_x - 0x60000) {
        int vel = *(int*)(c + 0xa8);
        if (vel < 0) {
            *(int*)(c + 0xa8) = -vel;
            *(short*)(c + 0xea) = 0;
            goto after_x;
        }
    }
    if (pos_x > base_x + 0x60000) {
        int vel = *(int*)(c + 0xa8);
        if (vel > 0) {
            *(int*)(c + 0xa8) = -vel;
            *(short*)(c + 0xea) = 0;
        }
    }
after_x:;
    }

    AddVec3((struct Vector3*)(c + 0x9c), (struct Vector3*)(c + 0xa8), (struct Vector3*)(c + 0x9c));
    if (*(int*)(c + 0xa8) > 0)
        _Z14ApproachLinearRsss((short*)(c + 0xe6), 0x3000, 0x200);
    else
        _Z14ApproachLinearRsss((short*)(c + 0xe6), -0x3000, 0x200);
    func_ov006_020c49d8(c);
}
