//cpp
// @symbol func_ov003_020b0814
// recovered name: dScGameOver_c_Render
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Stage.h"
/* recovered: renamed to Class_Method */
/* dScGameOver_c::Render - recovered from vtable slot identity */
extern "C" {
typedef int s32;
typedef int Fix12i;
typedef int bool32;
#include "OamAttr.h"
extern struct OamAttr *data_ov003_020b1824[];
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    bool32 sub, struct OamAttr *data, s32 x, s32 y,
    s32 palette, s32 priority, Fix12i scaleX, Fix12i scaleY,
    s32 rotation, s32 mode);
int func_ov003_020b0814(char *c);
int func_ov003_020b0814(char *c) {
    int i;
    for (i = 0; i < 8; i++) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, data_ov003_020b1824[i],
            ((short*)(c + 0x50))[i], ((short*)(c + 0x60))[i],
            -1, -1, 0x1000, 0x1000, 0, -1);
    }
    if (*(unsigned char*)(c + 0x94) >= 2)
        _ZN5Stage20RenderBouncingArrowsEv();
    return 1;
}
}
