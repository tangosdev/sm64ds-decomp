#ifndef ARMEDROTATINGPLATFORM_H
#define ARMEDROTATINGPLATFORM_H

#include "types.h"

/* A donut lift: stands still until ridden, then shakes, falls and respawns.
 *
 * Everything this header used to declare below 0x31e was Actor's and
 * Platform's, restated. It derives from Platform now, which the destructor
 * evidences the usual way -- it stores ArmedRotatingPlatform's vtable, then Platform's,
 * then destroys the MovingMeshCollider at 0x124 and the Model at 0xd4 and
 * chains to Actor. Those three are Platform's own.
 *
 * unk_31e sits at 0x31e, in Platform's TAIL PADDING: Platform's last field
 * ends at 0x31e and its size rounds to 0x320, and the Itanium ABI lets a
 * derived class place members in a non-POD base's tail padding. Behavior reads
 * this+0x31e and reproduces the ROM, which is what confirms the placement.
 */

#ifdef __cplusplus

#include "Platform.h"

struct ArmedRotatingPlatform : Platform {
    s16 unk_31e;            /* 0x31e - per-frame yaw step; see InitResources */

    /* --- vtable --- */
    virtual ~ArmedRotatingPlatform();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char DonutBlock_size_must_be_0x320[sizeof(ArmedRotatingPlatform) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* ARMEDROTATINGPLATFORM_H */
