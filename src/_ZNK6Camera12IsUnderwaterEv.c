#include "types.h"
/* Camera::IsUnderwater() const at 0x0200d890
 * Returns the IS_UNDERWATER (1<<0) bit of the camera flags word (@0x154).
 * Keeps the masked value (`flags & 1`) exactly as the original returns it.
 */
enum CameraFlags {
    IS_UNDERWATER = 1 << 0,
};

/* Partial Camera; flags lives at 0x154. */
struct Camera {
    char _pad00[0x154];
    u32 flags; /* 0x154 */
};

int _ZNK6Camera12IsUnderwaterEv(const struct Camera *self)
{
    return self->flags & IS_UNDERWATER;
}
