typedef int s32;
typedef int Fix12i;
typedef int bool32;

#include "OamAttr.h"

extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    bool32 sub, struct OamAttr *data,
    s32 x, s32 y,
    s32 palette, s32 priority,
    Fix12i scaleX, Fix12i scaleY,
    s32 rotation, s32 mode);

extern struct OamAttr data_ov075_0211cba4[];

void func_ov075_02118cd0(void)
{
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0, data_ov075_0211cba4, 0x80, 0x20,
        -1, -1,
        0x1000, 0x1000,
        0, -1);
}
