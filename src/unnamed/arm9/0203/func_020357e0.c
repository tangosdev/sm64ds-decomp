#include "types.h"
/* func_020357e0
 * Attempt 1: calls dBgCh::StopDetectingWater on this+0x134 then this+0x20.
 */
struct dBgCh { u32 _pad[1]; };

extern void func_02035414(struct dBgCh* self);

struct dBgCh_Actr {
    u32 pad00[8];           /* 0x00-0x1f */
    struct dBgCh fld20;      /* 0x20 */
    u32 pad24[68];          /* 0x24-0x133 */
    struct dBgCh fld134;     /* 0x134 */
};

void func_020357e0(struct dBgCh_Actr* self)
{
    func_02035414(&self->fld134);
    func_02035414(&self->fld20);
}
