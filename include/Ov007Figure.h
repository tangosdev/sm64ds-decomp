#ifndef OV007_FIGURE_H
#define OV007_FIGURE_H

#include "types.h"

/* A reconstructed layout and name, not recovered class identity. */

struct Ov007Model;

/* The 0x1c-byte figure func_ov007_020beaa0 builds around an Ov007Model:
 * Ov007Work's figure (index 0) and altFigure (index 1);
 * func_ov007_020be850 returns one of them. */
typedef struct Ov007Figure Ov007Figure;
struct Ov007Figure {
    u8 index;       /* 0x00: 0 for Ov007Work::figure, 1 for altFigure */
    u8 figureIndex; /* 0x01: a copy of Ov007Work's figureIndex */
    /* 0x02: func_ov007_020beaa0 seeds it (index != 0);
     * func_ov007_020beb80 and func_ov007_020be0dc hand it to
     * func_ov007_020b7690, the particle system lookup */
    u8 particleIndex;
    char pad_03;
    struct Ov007Model *model; /* 0x04: func_ov007_020bc6d4 */
    /* 0x08: bone 0's yaw, from FX_Atan2 (atan2) of its third rotation
     * row; func_ov007_020beb80 refreshes it on the stepped frames */
    int yaw;
    Vector3 prevPos; /* 0x0c: bone 0's translation on the last stepped frame */
    /* 0x18: the per-frame hook func_ov007_020be9ac calls while it is set;
     * func_ov007_020b0a20 installs func_ov007_020b0da0 there, which clears
     * it when its motion ends */
    void (*update)(Ov007Figure *);
};

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007Figure_size_must_be_0x1c[sizeof(Ov007Figure) == 0x1c ? 1 : -1];
#endif

#endif
