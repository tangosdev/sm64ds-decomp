#include "types.h"
/* Actor::DropShadowRadHeight — drops a shadow with radius/depth (not XYZ).
 * Reads flags@0xb0, tests OFF_SHADOW_RANGE (bit4=0x10).
 * If flag is set, returns immediately; otherwise calls ShadowModel::InitModel
 * with radius for scaleX and scaleZ, depth for scaleY.
 *
 * Disasm shows: args = (this, shadow, matrix, radius, depth, opacity)
 * Callee: 0x02015e64 = ShadowModel::InitModel(Matrix4x3*, Fix12i, Fix12i, Fix12i, u8)
 *   _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j
 */
struct Matrix4x3;
struct ShadowModel;

struct Actor {
    char _pad[0xb0];
    u32 flags;
};

extern void _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(
    struct ShadowModel* shadow, struct Matrix4x3* matrix,
    Fix12i scaleX, Fix12i scaleY, Fix12i scaleZ, u8 opacity);

void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    struct Actor* this_,
    struct ShadowModel* shadow,
    struct Matrix4x3* matrix,
    Fix12i radius,
    Fix12i depth,
    u8 opacity)
{
    if (this_->flags & 0x10)
        return;
    _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(shadow, matrix, radius, depth, radius, opacity);
}
