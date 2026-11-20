#ifndef OV007_MESH_H
#define OV007_MESH_H

#include "types.h"
#include "common.h"
#include "Ov007Spring.h"
#include "Ov007MenuState.h"

/* Reconstructed names and layout, not an original class identity. */

/* The 0x18-byte mesh template (data_ov007_020d7cb0) that
 * func_ov007_020cbbb0 copies whole into the mesh. */
typedef struct Ov007MeshParams {
    int pointMass; /* 0x00: Q12, every grid point (func_ov007_020c73f0) */
    /* 0x04/0x08: the mesh's spring, for the grid's rows and columns
     * (func_ov007_020c7240) and for each cell link's z (func_ov007_020c7a84) */
    int stiffness;
    int damping;
    /* 0x0c/0x10: the z force func_ov007_020ca578 adds to a poked point, at
     * strength 0 and at full strength (0x1000) */
    int pushMin;
    int pushMax;
    int restZ; /* 0x14: the z func_ov007_020cbbb0 builds every point at */
} Ov007MeshParams;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007MeshParams_size_must_be_0x18[sizeof(Ov007MeshParams) == 0x18 ? 1 : -1];
#endif

struct Ov007Camera;

/* The 0x38-byte grid mesh that func_ov007_020cbbb0 builds and
 * func_ov007_020cbb04 frees (sMeshScene[0]): a grid of spring
 * points with a link across each cell's diagonal, and the normals and
 * texture scratch func_ov007_020ca5f0 draws it with. */
typedef struct Ov007Mesh {
    VecFx32 *verts;  /* 0x00: one per grid point; each grid point's target */
    Ov007Grid *grid; /* 0x04: func_ov007_020c73f0 */
    /* 0x08: one func_ov007_020c80a4 link per cell, (columns - 1) *
     * (rows - 1) of them */
    Ov007Link **cellLinks;
    Ov007MeshParams params; /* 0x0c */
    s16 cellWidth;          /* 0x24: column spacing */
    s16 cellHeight;         /* 0x26: row spacing */
    /* 0x28: two per cell, one per triangle (func_ov007_020cb7c0) */
    Vector3_16 *faceNormals;
    Vector3_16 *vertexNormals; /* 0x2c: one per point (func_ov007_020cb4b0) */
    int *texOffsetS;           /* 0x30: func_ov007_020caeac's per-column tu scratch */
    int *texOffsetT;           /* 0x34: its tv scratch */
#ifdef __cplusplus
    int func_ov007_020bf630(struct Ov007Camera *camera, int x, int y, int maxDist);
#endif
} Ov007Mesh;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007Mesh_size_must_be_0x38[sizeof(Ov007Mesh) == 0x38 ? 1 : -1];
#endif

/* The 0x2c-byte texture record func_ov007_020c15ec fills (see unit020c1404). */
struct Ov007Texture;

/* The 0x34-byte material func_ov007_020cbf8c copies from a template
 * (sMeshScene[1] and [2]) and func_ov007_020ca5f0 draws the mesh
 * with. */
typedef struct Ov007MeshMaterial {
    struct Ov007Texture *texture; /* 0x00: one of Ov007Work's textures, or none */
    s32 texS;                     /* 0x04: func_ov007_020caeac u0 */
    s32 texT;                     /* 0x08: v0 */
    s32 warpS;       /* 0x0c: Q12; times cellWidth, how far a normal's x shifts the texcoord */
    s32 warpT;       /* 0x10: likewise for y and cellHeight */
    s32 tilesS;      /* 0x14: texture widths across the mesh */
    s32 tilesT;      /* 0x18: texture heights down it */
    s32 texRepeat;   /* 0x1c: func_ov007_020c1448 `repeat` */
    s32 texColor0;   /* 0x20: func_ov007_020c1448 `color0` */
    s32 alpha;       /* 0x24: POLYGON_ATTR bits 16-20; 0x1f = opaque */
    u16 diffuse;     /* 0x28: DIF_AMB low half; 0x7fff = white */
    u16 ambient;     /* 0x2a: DIF_AMB high half */
    s32 polygonMode; /* 0x2c: POLYGON_ATTR bits 4-5 */
    s32 polygonId;   /* 0x30: POLYGON_ATTR bits 24-29 */
} Ov007MeshMaterial;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007MeshMaterial_size_must_be_0x34[sizeof(Ov007MeshMaterial) == 0x34 ? 1 : -1];
#endif

/* data_ov007_020d7c9c, the wave template; chances are per mille. */
typedef struct Ov007MeshWaveParams {
    int speed;      /* 0x00: phase advance per tick of the wave's state */
    int spread;     /* 0x04: phase across one whole row or column */
    int amplitude;  /* 0x08: Q12, times the sine */
    u16 minIdle;    /* 0x0c: ticks a wave stays idle (-1) before it may re-roll */
    u16 keepChance; /* 0x0e: chance each tick a running wave keeps pushing */
    u16 idleChance; /* 0x10: chance a re-roll goes idle instead of picking a line */
} Ov007MeshWaveParams;

#ifndef SM64DS_PLATFORM_PC
/* data_ov007_020d7c9c runs 0x14 bytes to the next symbol. */
typedef char Ov007MeshWaveParams_size_must_be_0x14[sizeof(Ov007MeshWaveParams) == 0x14 ? 1 : -1];
#endif

/* The 0x1c-byte record func_ov007_020ca2a8 builds and func_ov007_020ca1d4
 * ticks: waves[0] runs down a mesh column and waves[1] along a row (2 and 3
 * are ticked but drive nothing). Each wave's currentState is its column or
 * row, -1 when idle, and its stateTicks the sine's time. */
typedef struct Ov007MeshWave {
    int frame;                   /* 0x00: func_ov007_020ca1d4 counts it up every call */
    int delay;                   /* 0x04: no wave runs until it counts down to 0 */
    Ov007MenuState *waves[4];    /* 0x08: func_ov007_020c9388(-2) */
    Ov007MeshWaveParams *params; /* 0x18: func_ov007_020ca2a8's argument */
} Ov007MeshWave;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007MeshWave_size_must_be_0x1c[sizeof(Ov007MeshWave) == 0x1c ? 1 : -1];
#endif

#endif
