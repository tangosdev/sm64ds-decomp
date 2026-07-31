#include "types.h"
/* _ZN12WithMeshClsn19StartDetectingWaterEv
 * Attempt 1: calls BgCh::StartDetectingWater on this+0x134 then this+0x20.
 */
struct BgCh { u32 _pad[1]; };

extern void _ZN4BgCh19StartDetectingWaterEv(struct BgCh* self);

struct WithMeshClsn {
    u32 pad00[8];           /* 0x00-0x1f */
    struct BgCh fld20;      /* 0x20 */
    u32 pad24[68];          /* 0x24-0x133 */
    struct BgCh fld134;     /* 0x134 */
};

void _ZN12WithMeshClsn19StartDetectingWaterEv(struct WithMeshClsn* self)
{
    _ZN4BgCh19StartDetectingWaterEv(&self->fld134);
    _ZN4BgCh19StartDetectingWaterEv(&self->fld20);
}