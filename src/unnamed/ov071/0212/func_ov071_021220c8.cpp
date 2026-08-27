//cpp
// @symbol func_ov071_021220c8
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

extern "C" void _Z14ApproachLinearRsss(short &dst, short a, short b);
extern "C" unsigned short DecIfAbove0_Short(unsigned short *p);
extern "C" void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int n, Vector3 *v);

extern "C" void func_ov071_021220c8(char *c)
{
    if (*(short *)(c + 0x8c) != 0x4000) {
        _Z14ApproachLinearRsss(*(short *)(c + 0x326), 0x3e8, 0xc8);
        _Z14ApproachLinearRsss(*(short *)(c + 0x8c), 0x4000, *(short *)(c + 0x326));
        return;
    }
    DecIfAbove0_Short((unsigned short *)(c + 0x324));
    if (*(unsigned short *)(c + 0x324) == 0) {
        func_ov071_021223c8(c, 0);
        *(short *)(c + 0x90) = 0;
        return;
    }
    if (*(unsigned short *)(c + 0x324) > 0x1e)
        return;
    if (*(unsigned short *)(c + 0x328) % 4 == 0)
        _ZN5Sound9PlayBank3EjRK7Vector3(0x59, (Vector3 *)(c + 0x74));
    *(short *)(c + 0x90) = (*(unsigned short *)(c + 0x328) & 1) * 0x190 - 0xc8;
}
