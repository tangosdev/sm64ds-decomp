#include "types.h"
/* WithMeshClsn::IsOnGround() const at 0x020356e8
 * Reads the ON_GROUND flag (1 << 4 == 0x10) of this->flags (u32 at 0x10).
 */
struct WithMeshClsn { char pad[0x10]; u32 flags; };

s32 _ZNK12WithMeshClsn10IsOnGroundEv(const struct WithMeshClsn* self)
{
    return self->flags & 16; // flags & ON_GROUND
}
