#include "types.h"
/* func_020357a0
 * Attempt 1: calls BgCh::StopDetectingWater on this+0x134 then this+0x20.
 */
struct BgCh { u32 _pad[1]; };

extern void func_020353e0(struct BgCh* self);

struct WithMeshClsn {
    u32 pad00[8];           /* 0x00-0x1f */
    struct BgCh fld20;      /* 0x20 */
    u32 pad24[68];          /* 0x24-0x133 */
    struct BgCh fld134;     /* 0x134 */
};

void func_020357a0(struct WithMeshClsn* self)
{
    func_020353e0(&self->fld134);
    func_020353e0(&self->fld20);
}
