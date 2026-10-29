//cpp
#include "types.h"
#include "OAM.h"
#include "OamAttr.h"

extern "C" {

/* func_0203083c at 0x0203083c, size=0x6c
 * If data_0209fc68 is non-null, looks up an OamAttr* in a table indexed by
 * func_0203d9f4(), then calls OAM::Render(false, oam, 8, 0xb8, ...).
 * Always calls empty stub func_02030838 at end.
 */

extern void *data_0209fc68;             /* 0x0209fc68: global pointer, checked for non-null */
extern struct OamAttr *data_020927a0[];  /* 0x020927a0: table of OamAttr* */

extern s32 func_0203d9f4(void);    /* 0x0203d9f4 */
extern void func_02030838(void);   /* 0x02030838 (empty stub, bx lr) */

void func_0203083c(void)
{
    if (data_0209fc68 != 0) {
        struct OamAttr *oam = data_020927a0[func_0203d9f4()];
        OAM::Render(false, oam, 8, 0xb8, -1, -1, 0x1000, 0x1000, 0, -1);
    }
    func_02030838();
}
}
