#include "types.h"
/* WithMeshClsn::JustHitGround() const at 0x0203571c
 * Reads the JUST_HIT_GROUND flag (1 << 5 == 0x20) of this->flags (u32 at 0x10).
 */
struct WithMeshClsn { char pad[0x10]; u32 flags; };

s32 _ZNK12WithMeshClsn13JustHitGroundEv(const struct WithMeshClsn* self)
{
    return self->flags & 32; // flags & JUST_HIT_GROUND
}
