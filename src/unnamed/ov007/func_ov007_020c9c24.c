#include "types.h"
#include "nitro/fx/fx.h"
#pragma opt_common_subs off

extern void func_ov007_020c7a84(char *p, int f1, int f2);
extern void func_ov007_020c80c8(char *o, int a, int s);
extern void func_ov007_020c7560(void *self, int r6, int r5);
extern void func_ov007_020c7504(int *self, int b, int c);

/* Reconstructed layouts and names, not recovered class identity; only the
 * slots this file touches.
 *
 * Ov007Mesh is data_ov007_02104bd4[0], the grid mesh func_ov007_020cbbb0
 * builds (names as in func_ov007_020ca5f0). */
typedef struct Ov007Normal {
    s16 x, y, z;
} Ov007Normal;

typedef struct Ov007Mesh {
    char pad_00[0x2c];
    Ov007Normal *vertexNormals; /* 0x2c: one per point (func_ov007_020cb4b0) */
} Ov007Mesh;

/* A point and a point list as func_ov007_020c5a6c spells them. */
typedef struct Ov007Point {
    Vector3 *pos; /* 0x0 */
} Ov007Point;

typedef struct Ov007PointList {
    Ov007Point **items; /* 0x0 */
} Ov007PointList;

/* The template func_ov007_020ca010 builds every Ov007Swing from
 * (data_ov007_020d7cc8). */
typedef struct Ov007SwingParams {
    char pad_00[8];
    /* 0x08/0x0c: the chain's spring, through func_ov007_020c7560 (mode 2,
     * func_ov007_020c7b2c: stretch, then velocity factor) */
    s32 chainStiffness;
    s32 chainDamping;
    s32 normalScale; /* 0x10: the anchor's x/y offset per unit of mesh normal */
    char pad_14[4];
    /* 0x18/0x1c: the side links' z spring (func_ov007_020c7a84) */
    s32 sideStiffness;
    s32 sideDamping;
    char pad_20[0xc];
    s16 maxNormalX; /* 0x2c: the mesh normal's x is clamped to +-maxNormalX */
    s16 maxNormalY; /* 0x2e */
} Ov007SwingParams;

typedef char Ov007SwingParams_size_must_be_0x30[sizeof(Ov007SwingParams) == 0x30 ? 1 : -1];

/* The 0x70-byte record func_ov007_020ca010 builds and func_ov007_020c9fa0
 * frees, one per menu item (data_ov007_02104bd4[4..11]); medium-confidence
 * name. A chain of points hangs along z from a pinned first point, and two
 * pairs of side points (spread along x and along y) are tied to the mesh by
 * links (func_ov007_020c9d90). func_ov007_020c9a84 turns the chain's last
 * point and the side pairs' slopes into the item sprite's offset and tilt. */
typedef struct Ov007Swing {
    Ov007Mesh *mesh;          /* 0x00: func_ov007_020c9d90 binds it */
    Ov007SwingParams *params; /* 0x04 */
    int meshPoint;            /* 0x08: the mesh point nearest the anchor (func_ov007_020c9d90) */
    char pad_0c[0x10];
    /* 0x1c: func_ov007_020c798c point list; its first Point is pinned
     * (func_ov007_020ca010) and this file moves it */
    Ov007PointList *chain;
    char pad_20[0x30];
    void *xPoints[2]; /* 0x50: the side points spread along x (func_ov007_020c844c) */
    void *yPoints[2]; /* 0x58: spread along y */
    void *xLinks[2];  /* 0x60: xPoints[i] to the mesh (func_ov007_020c9d90) */
    void *yLinks[2];  /* 0x68 */
} Ov007Swing;

typedef char Ov007Swing_size_must_be_0x70[sizeof(Ov007Swing) == 0x70 ? 1 : -1];

/* One simulation step: the side points spring along z, the chain's pinned
 * first point follows the mesh normal under it, then the chain springs and
 * integrates. o keeps the int * spelling of the definition; `self` is the
 * typed view. */
void func_ov007_020c9c24(int *o, int arg1) {
    Ov007Swing *self = (Ov007Swing *)o;
    int i;
    int bnd;
    int nx, ny;
    int scale;
    Ov007Normal *n;
    Ov007SwingParams *ctx;

    for (i = 0; i < 2; i++) {
        ctx = self->params;
        func_ov007_020c7a84((char *)self->xLinks[i], ctx->sideStiffness, ctx->sideDamping);
        func_ov007_020c80c8((char *)self->xPoints[i], 0, arg1);
        ctx = self->params;
        func_ov007_020c7a84((char *)self->yLinks[i], ctx->sideStiffness, ctx->sideDamping);
        func_ov007_020c80c8((char *)self->yPoints[i], 0, arg1);
    }

    n = &self->mesh->vertexNormals[self->meshPoint];
    ctx = self->params;
    bnd = ctx->maxNormalX;
    nx = n->x;
    ny = n->y;

    if (nx > bnd)
        nx = bnd;
    else if (nx < -bnd)
        nx = (s16)(-bnd);

    bnd = ctx->maxNormalY;
    if (ny > bnd)
        ny = bnd;
    else if (ny < -bnd)
        ny = (s16)(-bnd);

    scale = ctx->normalScale;
    self->chain->items[0]->pos->x = FX_MUL(nx, scale);
    scale = self->params->normalScale;
    self->chain->items[0]->pos->y = FX_MUL(ny, scale);

    ctx = self->params;
    func_ov007_020c7560(self->chain, ctx->chainStiffness, ctx->chainDamping);
    func_ov007_020c7504((int *)self->chain, 0, arg1);
}
