#include "types.h"
extern void* func_ov007_020c3df4(int a, int b);

/* The 0x34-byte grid-mesh material func_ov007_020ca5f0 draws with; names
 * and evidence as in that file's view. */
typedef struct Ov007MeshMaterial {
    int *texture;    /* 0x00: a manager texture record */
    s32 texS;        /* 0x04 */
    s32 texT;        /* 0x08 */
    s32 warpS;       /* 0x0c */
    s32 warpT;       /* 0x10 */
    s32 tilesS;      /* 0x14 */
    s32 tilesT;      /* 0x18 */
    s32 texRepeat;   /* 0x1c */
    s32 texColor0;   /* 0x20 */
    s32 alpha;       /* 0x24: POLYGON_ATTR alpha, 0x1f = opaque */
    u16 diffuse;     /* 0x28: DIF_AMB low half, 0x7fff = white */
    u16 ambient;     /* 0x2a */
    s32 polygonMode; /* 0x2c */
    s32 polygonId;   /* 0x30 */
} Ov007MeshMaterial;

typedef char Ov007MeshMaterial_size_must_be_0x34[sizeof(Ov007MeshMaterial) == 0x34 ? 1 : -1];

/* Allocates a material, copied from src when given, else the defaults:
 * opaque, white diffuse, no texture. */
void* func_ov007_020cbf8c(void* src)
{
    Ov007MeshMaterial *mat = (Ov007MeshMaterial *)func_ov007_020c3df4(0, 0x34);
    if (src != 0) {
        *mat = *(Ov007MeshMaterial *)src;
    } else {
        mat->texture = 0;
        mat->texT = 0;
        mat->texS = mat->texT;
        mat->warpT = 0;
        mat->warpS = mat->warpT;
        mat->alpha = 0x1f;
        mat->diffuse = 0x7fff;
        mat->ambient = 0;
        mat->polygonMode = 0;
        mat->polygonId = 0;
        mat->texRepeat = 0;
        mat->texColor0 = 0;
    }
    return mat;
}
