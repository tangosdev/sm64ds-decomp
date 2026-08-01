#include "types.h"
/* Actor::DropShadowScaleXYZ — drops a shadow with separate X/Y/Z scales.
 * Reads flags@0xb0, tests OFF_SHADOW_RANGE (bit4=0x10).
 * If flag is set, returns immediately; otherwise calls ShadowModel::InitModel.
 *
 * Callee: 0x02015e64 = ShadowModel::InitModel(Matrix4x3*, Fix12i, Fix12i, Fix12i, u32)
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

void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    struct Actor* this_,
    struct ShadowModel* shadow,
    struct Matrix4x3* matrix,
    Fix12i scaleX,
    Fix12i scaleY,
    Fix12i scaleZ,
    u8 opacity)
{
    if (this_->flags & 0x10)
        return;
    _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(shadow, matrix, scaleX, scaleY, scaleZ, opacity);
}
