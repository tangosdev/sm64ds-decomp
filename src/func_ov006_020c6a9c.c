// @symbol func_ov006_020c6a9c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef short s16;
typedef unsigned short u16;



extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int _Z15ApproachLinear2Rsss(short *v, short target, short step);


void func_ov006_020c6a9c(char *c)
{
    short *ang = (short *)(((int)c + 0xea));
    *ang = *ang + 0x200;

    *(int *)(c + 0xa8) = (int)(((*(int *)(c + 0x20) - *(int *)(c + 0x9c)) * 0x100LL + 0x800) >> 12);
    *(int *)(c + 0xac) = (int)(((*(int *)(c + 0x24) - *(int *)(c + 0xa0)) * 0x100LL + 0x800) >> 12);
    *(int *)(c + 0xb0) = (int)(((*(int *)(c + 0x28) - *(int *)(c + 0xa4)) * 0x100LL + 0x800) >> 12);
    AddVec3((struct Vector3 *)(c + 0x9c), (struct Vector3 *)(c + 0xa8), (struct Vector3 *)(c + 0x9c));

    if ((*(int *)(c + 0xa8) < 0 ? -*(int *)(c + 0xa8) : *(int *)(c + 0xa8)) < 0x10 &&
        (*(int *)(c + 0xac) < 0 ? -*(int *)(c + 0xac) : *(int *)(c + 0xac)) < 0x10) {
        *(int *)(c + 0x9c) = *(int *)(c + 0x20);
        *(int *)(c + 0xa0) = *(int *)(c + 0x24);
        *(int *)(c + 0xa4) = *(int *)(c + 0x28);

        switch (*(int *)(c + 0x2c)) {
        case 0:  func_ov006_020c63dc(c); break;
        case 1:  func_ov006_020c6348(c); break;
        case 2:  func_ov006_020c6248(c); break;
        case 3:  func_ov006_020c6188(c); break;
        case 4:  func_ov006_020c5ec8(c); break;
        case 5:  func_ov006_020c5cf4(c); break;
        case 6:  func_ov006_020c5aa4(c); break;
        case 7:  func_ov006_020c57d4(c); break;
        case 8:  func_ov006_020c561c(c); break;
        case 9:  func_ov006_020c54f4(c); break;
        case 10: func_ov006_020c5370(c); break;
        default: func_ov006_020c63dc(c); break;
        }
    }

    _Z15ApproachLinear2Rsss((short *)(c + 0xec), 0, 1);
    if (*(s16 *)(c + 0xec) != 1)
        return;
    func_ov006_020e6e3c(0x1c5, *(int *)(c + 0x9c));
}
