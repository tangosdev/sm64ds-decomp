#ifndef OV007_LIGHT_H
#define OV007_LIGHT_H

#include "types.h"
#include "nitro/fx/fx.h"

/* A reconstructed layout and name, not recovered class identity. */

/* The 0x28-byte light func_ov007_020c241c builds; func_ov007_020b6a84 keeps
 * two in Ov007Work::lights. func_ov007_020c2390 derives its direction and
 * func_ov007_020c232c loads it into the geometry engine. */
typedef struct Ov007Light {
    int id; /* 0x00: func_ov007_020c241c's argument; the hardware light, 0..3 */
    /* 0x04/0x10, medium confidence: the light shines from `source` towards
     * `target` (func_ov007_020c2390's direction is target - source).
     * func_ov007_020c241c seeds target at the origin and source at
     * data_ov007_020d7dac, (0, 0, 1.0); func_ov007_020bd1c0 moves source
     * per menu. */
    VecFx32 target;
    VecFx32 source;
    /* 0x1c: the s16 unit direction func_ov007_020c2390 derives from the two
     * points, each axis clamped to 0xfff */
    s16 dirX;
    s16 dirY;
    s16 dirZ;
    u16 color;   /* 0x22: func_ov007_020c241c seeds 0x7fff */
    int enabled; /* 0x24: func_ov007_020c241c sets 1; func_ov007_020c232c loads nothing while 0 */
} Ov007Light;

typedef char Ov007Light_size_must_be_0x28[sizeof(Ov007Light) == 0x28 ? 1 : -1];

#endif
