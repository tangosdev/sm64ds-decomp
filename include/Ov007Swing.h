#ifndef OV007_SWING_H
#define OV007_SWING_H

#include "types.h"
#include "Ov007Spring.h"
#include "Ov007Mesh.h"

/* Reconstructed names and layout, not an original class identity. */

struct Ov007Swing;

/* The 0x30-byte template func_ov007_020ca010 builds every swing from
 * (data_ov007_020d7cc8). */
typedef struct Ov007SwingParams {
    int chainLength;    /* 0x00: points in the chain */
    int chainPointMass; /* 0x04: Q12 */
    /* 0x08/0x0c: the chain's spring, through func_ov007_020c7560 (mode 2,
     * func_ov007_020c7b2c: stretch, then velocity factor) */
    s32 chainStiffness;
    s32 chainDamping;
    s32 normalScale;   /* 0x10: the anchor's x/y offset per unit of mesh normal */
    int sidePointMass; /* 0x14: Q12 */
    /* 0x18/0x1c: the side links' z spring (func_ov007_020c7a84) */
    s32 sideStiffness;
    s32 sideDamping;
    char pad_20[8];
    /* 0x28/0x2a: the largest rotX/rotY func_ov007_020c9a84 hands the item
     * sprite; the side pairs' slopes are eased up to them */
    u16 maxRotX;
    u16 maxRotY;
    s16 maxNormalX; /* 0x2c: the mesh normal's x is clamped to +-maxNormalX */
    s16 maxNormalY; /* 0x2e */
#ifdef __cplusplus
    Ov007Swing *func_ov007_020ca010();
#endif
} Ov007SwingParams;

typedef char Ov007SwingParams_size_must_be_0x30[sizeof(Ov007SwingParams) == 0x30 ? 1 : -1];

/* The 0x70-byte record func_ov007_020ca010 builds and func_ov007_020c9fa0
 * frees, one per menu item (sMeshScene[4..11]); medium-confidence
 * name. A chain of points hangs along z from a pinned first point, and two
 * pairs of side points (spread along x and along y) are tied to the mesh by
 * links (func_ov007_020c9d90). func_ov007_020c9a84 turns the chain's last
 * point and the side pairs' slopes into the item sprite's offset and tilt. */
typedef struct Ov007Swing {
    Ov007Mesh *mesh;          /* 0x00: func_ov007_020c9d90 binds it */
    Ov007SwingParams *params; /* 0x04 */
    int meshPoint;            /* 0x08: the mesh point nearest the anchor (func_ov007_020c9d90) */
    VecFx32 anchor;           /* 0x0c: func_ov007_020c9f10 places it */
    /* 0x18: one position per chain point, z = index << 12 apart at build;
     * each chain point's target */
    VecFx32 *chainPositions;
    /* 0x1c: func_ov007_020c798c point list; its first point is pinned
     * (func_ov007_020ca010) */
    Ov007PointList *chain;
    VecFx32 xPointPos[2];      /* 0x20: xPoints[i]'s targets, spread along x */
    VecFx32 yPointPos[2];      /* 0x38: yPoints[i]'s, spread along y */
    Ov007Follower *xPoints[2]; /* 0x50: func_ov007_020c844c */
    Ov007Follower *yPoints[2]; /* 0x58 */
    Ov007Link *xLinks[2];      /* 0x60: xPoints[i] to the mesh (func_ov007_020c9d90) */
    Ov007Link *yLinks[2];      /* 0x68 */
#ifdef __cplusplus
    void func_ov007_020c9fa0();
    void func_ov007_020c9f10(VecFx32 *pos, int w, int h);
    void func_ov007_020c9d90(Ov007Mesh *mesh);
    void func_ov007_020c9c24(int shift);
    void func_ov007_020c9a84(VecFx32 *offset, u16 *rotX, u16 *rotY);
#endif
} Ov007Swing;

typedef char Ov007Swing_size_must_be_0x70[sizeof(Ov007Swing) == 0x70 ? 1 : -1];

#endif
