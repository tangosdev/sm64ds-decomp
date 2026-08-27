//cpp
// @symbol _ZN13dScGameOver_c6RenderEv
/* recovered: named members + real C++ method */
/* dScGameOver_c::Render() -- vtable slot 9. Draws the eight "GAME OVER" glyph
 * sprites from the per-glyph x/y arrays InitResources laid out, then lets Stage
 * draw the yes/no bouncing arrows once the cursor is live. */
#include "dScGameOver_c.h"
#include "decl_Stage.h"
#include "OamAttr.h"

extern "C" {
extern OamAttr *data_ov003_020b1824[];
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    s32 sub, OamAttr *data, s32 x, s32 y,
    s32 palette, s32 priority, Fix12i scaleX, Fix12i scaleY,
    s32 rotation, s32 mode);
}

s32 dScGameOver_c::Render()
{
    int i;
    for (i = 0; i < 8; i++) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, data_ov003_020b1824[i],
            unk_050[i], unk_060[i],
            -1, -1, 0x1000, 0x1000, 0, -1);
    }
    if (unk_094 >= 2)
        _ZN5Stage20RenderBouncingArrowsEv();
    return 1;
}
