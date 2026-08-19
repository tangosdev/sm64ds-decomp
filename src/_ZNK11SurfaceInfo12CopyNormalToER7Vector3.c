/* SurfaceInfo::CopyNormalTo(Vector3& out) const at 0x02037dcc
 * Copies this->normal (Vector3 at 0x8, after the 8-byte CLPS) into out.
 */

#include "SurfaceInfo.h"

void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(const struct SurfaceInfo* self, struct Vector3* out)
{
    out->x = self->normal.x;
    out->y = self->normal.y;
    out->z = self->normal.z;
}
