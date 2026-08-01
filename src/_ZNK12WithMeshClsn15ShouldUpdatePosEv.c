#include "types.h"
/* WithMeshClsn::ShouldUpdatePos() const at 0x02035564
 * True unless the NO_UPDATE_POS flag (1 << 13 == 0x2000) is set in
 * this->flags (u32 at 0x10).
 */
struct WithMeshClsn { char pad[0x10]; u32 flags; };

s32 _ZNK12WithMeshClsn15ShouldUpdatePosEv(const struct WithMeshClsn* self)
{
    return (self->flags & 8192) == 0; // !(flags & NO_UPDATE_POS)
}
