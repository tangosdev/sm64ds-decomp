// @symbol _ZN3OAM9RenderSubEP7OamAttriiii
/* recovered: named members + shared header */
#include "OAM.h"
/* _ZN3OAM9RenderSubEP7OamAttriiii at 0x0202199c, size=0x54
 * OAM::RenderSub(OamAttri* data, s32 x, s32 y, s32 palette, s32 priority) -
 * Calls OAM::Render(true, data, x, y, palette, priority, 0x1000, 0x1000, 0, -1).
 */

typedef int s32;
typedef int Fix12i;
typedef int bool32;

struct OamAttri { unsigned short attr0, attr1, attr2, attr3; };

extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    bool32 sub, struct OamAttri *data,
    s32 x, s32 y,
    s32 palette, s32 priority,
    Fix12i scaleX, Fix12i scaleY,
    s32 rotation, s32 mode);

void _ZN3OAM9RenderSubEP7OamAttriiii(struct OamAttri *data, s32 x, s32 y, s32 palette, s32 priority)
{
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        1, data, x, y,
        palette, priority,
        0x1000, 0x1000,
        0, -1);
}
