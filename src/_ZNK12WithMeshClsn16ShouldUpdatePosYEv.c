#include "types.h"
/* WithMeshClsn::ShouldUpdatePosY() const at 0x02035578
 * True unless the NO_UPDATE_POS_Y flag (1 << 12 == 0x1000) is set in
 * this->flags (u32 at 0x10).
 */
struct WithMeshClsn { char pad[0x10]; u32 flags; };

s32 _ZNK12WithMeshClsn16ShouldUpdatePosYEv(const struct WithMeshClsn* self)
{
    return (self->flags & 4096) == 0; // !(flags & NO_UPDATE_POS_Y)
}
