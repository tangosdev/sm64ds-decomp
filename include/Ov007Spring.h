#ifndef OV007_SPRING_H
#define OV007_SPRING_H

#include "types.h"

/* Reconstructed names and layouts, not original class identities: the
 * overlay's mass-spring records. The touch picker (Ov007Picker.h) and the
 * menu's mesh effects (func_ov007_020ca308, func_ov007_020cbbb0) are built
 * from them. */

/* The 0x24-byte point mass that func_ov007_020c844c allocates and
 * func_ov007_020c83fc resets. Its steppers read every slot:
 * func_ov007_020c81a0 (x/y), func_ov007_020c80c8 (z) and func_ov007_020c831c
 * (3D). The vectors are typed Vector3 because the link steps hand velocity
 * and force to AddVec3, SubVec3, DotVec3 and func_02053320 (out = b + s * a,
 * per component). */
typedef struct Ov007Follower {
    /* 0x00: the Q12 position each step moves (func_ov007_020c844c's p). The
     * picker points it at pos, anchorPos or a groupPoints entry */
    Vector3 *target;
    Vector3 velocity; /* 0x04: added to *target every step, >> shift */
    Vector3 force;    /* 0x10: accumulated per frame, cleared at the end of each step */
    /* 0x1c: Q12. func_ov007_020c844c stores the reciprocal (FX_Inv)
     * of its mass, 0x1000 for a mass of 1.0. The steppers take a direct path
     * at 0x1000 and otherwise scale force by it into velocity. Signed, as
     * func_ov007_020c80c8 reads it (asr) */
    s16 invMass;
    u16 pad_1e;
    int flags; /* 0x20: bit 0 skips the step (target held) */
} Ov007Follower;

/* The 0x18-byte link func_ov007_020c80a4 allocates; func_ov007_020c8010
 * binds its ends. In the spring modes (func_ov007_020c7b2c for x/y,
 * func_ov007_020c7a84 for z) dir is the rest offset of `from` relative to
 * `to`. */
typedef struct Ov007Link {
    Ov007Follower *from; /* 0x00 */
    Ov007Follower *to;   /* 0x04 */
    int length;          /* 0x08: LenVec3(dir) */
    Vector3 dir;         /* 0x0c: from->target - to->target when bound */
} Ov007Link;

/* The 0x14-byte point list func_ov007_020c798c builds. */
typedef struct Ov007PointList {
    /* 0x00: the points. With flags bit 0 the list makes them itself;
     * otherwise the caller fills them in (func_ov007_020c73f0's column lists
     * share the row lists' points) */
    Ov007Follower **items;
    int count;         /* 0x04 */
    Ov007Link **links; /* 0x08: count - 1, or count when closed */
    int flags;         /* 0x0c: bit 0 makes its own points, bit 1 is closed */
    int springMode;    /* 0x10: which spring step func_ov007_020c7560 runs */
} Ov007PointList;

/* The 0x10-byte grid func_ov007_020c73f0 builds. */
typedef struct Ov007Grid {
    int columns;                  /* 0x00 */
    int rows;                     /* 0x04 */
    Ov007PointList **rowLists;    /* 0x08: one per row, columns points each */
    Ov007PointList **columnLists; /* 0x0c: one per column, rows points each */
} Ov007Grid;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007Follower_size_must_be_0x24[sizeof(Ov007Follower) == 0x24 ? 1 : -1];
typedef char Ov007Link_size_must_be_0x18[sizeof(Ov007Link) == 0x18 ? 1 : -1];
typedef char Ov007PointList_size_must_be_0x14[sizeof(Ov007PointList) == 0x14 ? 1 : -1];
typedef char Ov007Grid_size_must_be_0x10[sizeof(Ov007Grid) == 0x10 ? 1 : -1];
#endif

#endif
