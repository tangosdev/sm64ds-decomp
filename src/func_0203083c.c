/* func_0203083c at 0x0203083c, size=0x6c
 * If data_0209fc68 is non-null, looks up an OamAttr* in a table indexed by
 * func_0203d9f4(), then calls OAM::Render(false, oam, 8, 0xb8, ...).
 * Always calls empty stub func_02030838 at end.
 */

typedef int s32;
typedef int Fix12i;
typedef int bool32;

#include "OamAttr.h"

extern void *data_0209fc68;             /* 0x0209fc68: global pointer, checked for non-null */
extern struct OamAttr *data_020927a0[];  /* 0x020927a0: table of OamAttr* */

extern s32 func_0203d9f4(void);    /* 0x0203d9f4 */
extern void func_02030838(void);   /* 0x02030838 (empty stub, bx lr) */

extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    bool32 sub, struct OamAttr *data,
    s32 x, s32 y,
    s32 palette, s32 priority,
    Fix12i scaleX, Fix12i scaleY,
    s32 rotation, s32 mode);

void func_0203083c(void)
{
    if (data_0209fc68 != 0) {
        struct OamAttr *oam = data_020927a0[func_0203d9f4()];
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, oam,
            8, 0xb8,
            -1, -1,
            0x1000, 0x1000,
            0, -1);
    }
    func_02030838();
}
