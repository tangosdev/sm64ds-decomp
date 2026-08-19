//cpp
// @symbol _ZN13dScGameOver_c6RenderEv
/* recovered: renamed to Class_Method, RTTI class fields named, declarations
   from a shared header */
#include "decl_Stage.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScGameOver_c.h"
/* dScGameOver_c::Render() -- vtable slot 9. extern "C" carries the literal
 * mangled name unmangled -- see include/dScGameOver_c.h. */
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
int _ZN13dScGameOver_c6RenderEv(char *c);
int _ZN13dScGameOver_c6RenderEv(char *c) {
    dScGameOver_c *self = (dScGameOver_c *)(void *)c;
    int i;
    for (i = 0; i < 8; i++) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, data_ov003_020b1824[i],
            self->unk_050[i], self->unk_060[i],
            -1, -1, 0x1000, 0x1000, 0, -1);
    }
    if (self->unk_094 >= 2)
        _ZN5Stage20RenderBouncingArrowsEv();
    return 1;
}
}
