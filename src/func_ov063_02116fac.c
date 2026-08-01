#include "types.h"
// @symbol func_ov063_02116fac
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void func_0201267c(unsigned int id, void* p);
extern void _Z14ApproachLinearRiii(int* p, int target, int step);
extern short Vec3_HorzAngle(const void* a, const void* b);
extern void _Z14ApproachLinearRsss(short* p, short target, int step);


struct Frame {
    struct Vector3 v;
    int pad[10];
};

#define L16(c, off) ((u16*)(((int)(c) + (off))))
#define L8(c, off)  ((u8*)(((int)(c) + (off))))

void func_ov063_02116fac(char* c)
{
    struct Frame fr;
    int r4 = 0xc00;
    u16* flags;
    u8* state;

    fr.v.x = *(int*)(c + 0x51c);
    fr.v.y = *(int*)(c + 0x520);
    fr.v.z = *(int*)(c + 0x524);
    *(int*)(c + 0x584) = 0x1000;

    switch (*(u8*)(c + 0x5cc)) {
    case 0:
        flags = L16(c, 0x5d4);
        *flags &= ~8;
        if (((u32)(*(u16*)(c + 0x5d4) << 0x1b)) >> 0x1f != 0) {
            state = L8(c, 0x5cc);
            (*state)++;
            *(u8*)(c + 0x5c8) = 0xb4;
            *flags |= 8;
        }
        break;
    case 1:
        if (*(int*)(c + 0x580) < 0x258000) {
            state = L8(c, 0x5cc);
            (*state)++;
            func_0201267c(0xf8, c + 0x74);
        }
        *(int*)(c + 0x98) = 0;
        break;
    case 2:
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x3a000, 0x1800);
        fr.v.x = -0x3e8000;
        fr.v.y = 0;
        fr.v.z = 0xfea84000;
        if (*(int*)(c + 0x64) < (int)0xfee08000) {
            state = L8(c, 0x5cc);
            (*state)++;
        }
        break;
    case 3:
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x9000);
        fr.v.x = -0x3e8000;
        fr.v.y = 0;
        fr.v.z = -0xfa0000;
        *(s16*)(c + 0x5bc) = Vec3_HorzAngle(c + 0x5c, &fr.v);
        r4 = 0x1000;
        if (*(s16*)(c + 0x8e) == *(s16*)(c + 0x5bc) && *(int*)(c + 0x98) == 0) {
            state = L8(c, 0x5cc);
            (*state)++;
        }
        break;
    case 4:
        if (*(u16*)(c + 0x100) == 6) {
            *(int*)(c + 0xa8) = 0xf000;
            *(int*)(c + 0x9c) = -0x4000;
            *(int*)(c + 0xa0) = -0xf000;
        }
        if (*(int*)(c + 0x60) < *(int*)(c + 0x520)) {
            *(int*)(c + 0x60) = *(int*)(c + 0x520);
            state = L8(c, 0x5cc);
            (*state)++;
            *(int*)(c + 0x9c) = 0;
            *(int*)(c + 0xa0) = 0;
        }
        break;
    case 5:
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x3a000, 0x1800);
        fr.v.x = -0x3e8000;
        fr.v.y = 0;
        fr.v.z = 0xfea84000;
        r4 = 0x1000;
        if (*(int*)(c + 0x64) < (int)0xfec78000) {
            state = L8(c, 0x5cc);
            (*state)++;
        }
        break;
    case 6:
        *(int*)(c + 0x98) = 0;
        *(u8*)(c + 0x5c8) = 0;
        flags = L16(c, 0x5d4);
        *flags &= ~8;
        break;
    }

    if (*(u8*)(c + 0x5cc) != 4) {
        *(int*)(c + 0xa8) = 0;
        *(s16*)(c + 0x5bc) = Vec3_HorzAngle(c + 0x5c, &fr.v);
    }
    _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x5bc), r4);
    func_ov063_0211a964(c, 1);
}
