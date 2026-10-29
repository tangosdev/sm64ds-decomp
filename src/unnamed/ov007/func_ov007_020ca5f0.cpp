//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "cstd.h"

extern "C" {

extern void func_ov007_020c1448(void *a, int b, int c, int d, int e);
extern void func_ov007_020c1404(void *c);
extern void func_ov007_020caeac(int a0, int a1, int a2, int a3, int s0, int s1, int s2, int s3,
                                int s4, int s5, int s6, int s7, int s8);
extern void func_ov007_020ca86c(int a0, int a1, int a2, int a3, int s0);
/* Reconstructed layouts and names, not recovered class identity; only the
 * slots this file reads. a1 is data_ov007_02104bd4[0], the 0x38-byte grid
 * mesh func_ov007_020cbbb0 builds (func_ov007_020caeac/020ca86c emit it);
 * a2 is data_ov007_02104bd4[1] or [2], a 0x34-byte material record
 * func_ov007_020cbf8c copies from a template (see func_ov007_020bee14). */

/* The 0x10-byte grid func_ov007_020c73f0 builds; names as in
 * func_ov007_020cbb04, only the slots this file reads. */
typedef struct Ov007Grid {
    int columns; /* 0x00 */
    int rows;    /* 0x04 */
} Ov007Grid;

/* The 0x2c-byte texture record func_ov007_020c15ec fills; names as in that
 * file, only the slots this file reads. */
typedef struct Ov007Texture {
    char pad_00[0x14];
    int sizeS; /* 0x14: log2(width / 8); func_ov007_020c1448's name */
    int sizeT; /* 0x18: log2(height / 8) */
} Ov007Texture;

typedef struct Ov007Mesh {
    void *verts;     /* 0x00: Vector3 per grid point (func_ov007_020cbbb0) */
    Ov007Grid *grid; /* 0x04: func_ov007_020c73f0 */
    /* 0x08: one func_ov007_020c80a4 link per cell
     * (func_ov007_020cba78); 0x0c..0x23 hold func_ov007_020cbbb0's template
     * copy, with the spring's stiffness/damping at 0x10/0x14 */
    void **cellLinks;
    char pad_0c[4];
    int stiffness; /* 0x10 */
    int damping;   /* 0x14 */
    char pad_18[0xc];
    s16 cellWidth;       /* 0x24: func_ov007_020cbbb0's column spacing */
    s16 cellHeight;      /* 0x26: row spacing */
    void *faceNormals;   /* 0x28: s16 triple per triangle, two per cell (func_ov007_020cb7c0) */
    void *vertexNormals; /* 0x2c: s16 triple per point (func_ov007_020cb4b0) */
    void *texOffsetS;    /* 0x30: func_ov007_020caeac's per-column tu scratch */
    void *texOffsetT;    /* 0x34: its tv scratch */
} Ov007Mesh;

typedef struct Ov007MeshMaterial {
    Ov007Texture *texture; /* 0x00: a manager texture record */
    s32 texS;        /* 0x04: func_ov007_020caeac u0 */
    s32 texT;        /* 0x08: v0 */
    s32 warpS;       /* 0x0c: Q12; times cellWidth, how far a normal's x shifts the texcoord */
    s32 warpT;       /* 0x10: likewise for y and cellHeight */
    s32 tilesS;      /* 0x14: texture widths across the mesh */
    s32 tilesT;      /* 0x18: texture heights down it */
    s32 texRepeat;   /* 0x1c: func_ov007_020c1448 `repeat` (Ov007Sprite's name) */
    s32 texColor0;   /* 0x20: func_ov007_020c1448 `color0` */
    s32 alpha;       /* 0x24: POLYGON_ATTR bits 16-20; func_ov007_020cbf8c defaults it to 0x1f */
    u16 diffuse;     /* 0x28: DIF_AMB low half; defaults to 0x7fff */
    u16 ambient;     /* 0x2a: DIF_AMB high half */
    s32 polygonMode; /* 0x2c: POLYGON_ATTR bits 4-5 */
    s32 polygonId;   /* 0x30: POLYGON_ATTR bits 24-29 */
} Ov007MeshMaterial;

typedef char Ov007Mesh_size_must_be_0x38[sizeof(Ov007Mesh) == 0x38 ? 1 : -1];
typedef char Ov007MeshMaterial_size_must_be_0x34[sizeof(Ov007MeshMaterial) == 0x34 ? 1 : -1];

/* a1/a2 keep the char * spelling of the callers' declarations; mesh/mat are
 * the typed views. */
void func_ov007_020ca5f0(char *a1, char *a2, int *a3, int a4, int a5) {
    Ov007Mesh *mesh = (Ov007Mesh *)a1;
    Ov007MeshMaterial *mat = (Ov007MeshMaterial *)a2;
    Ov007Texture *r7 = mat->texture;
    Ov007Grid *pdim = mesh->grid;
    int d0 = pdim->columns - 1;
    int d1 = pdim->rows - 1;
    int tw = 0;
    int th = 0;
    *((volatile u32 *)0x40004c0) = (((u32)mat->diffuse) | (((u32)mat->ambient) << 16)) | 0x8000;
    *((volatile u32 *)0x40004c4) = 0;
    if (r7 != 0) {
        tw = 8 << r7->sizeS;
        th = 8 << r7->sizeT;
    }
    func_ov007_020c1448(r7, 1, mat->texRepeat, 0, mat->texColor0);
    func_ov007_020c1404(r7);
    *((volatile u32 *)0x40004a4) =
        ((0x8c1 | (mat->polygonMode << 4)) | (mat->polygonId << 24)) | (mat->alpha << 16);
    *((volatile u32 *)0x4000444) = 0;
    {
        int t2 = a3[2];
        int t1 = a3[1];
        int t0 = a3[0];
        *((volatile u32 *)0x4000470) = t0;
        *((volatile u32 *)0x4000470) = t1;
        *((volatile u32 *)0x4000470) = t2;
    }
    *((volatile u32 *)0x400046c) = 0x1000;
    *((volatile u32 *)0x400046c) = 0x1000;
    *((volatile u32 *)0x400046c) = a4;
    *((volatile u32 *)0x4000444) = 0;
    *((volatile u32 *)0x400046c) = 0x40000;
    *((volatile u32 *)0x400046c) = 0x40000;
    *((volatile u32 *)0x400046c) = 0x40000;
    if (a5 != 0) {
        int q1 = cstd::div(tw * mat->tilesS, d0);
        int q2 = cstd::div(th * mat->tilesT, d1);
        int fx1 = FX_MUL(mesh->cellWidth, mat->warpS);
        int fx2 = FX_MUL(mesh->cellHeight, mat->warpT);
        func_ov007_020caeac((int)mesh->verts, (int)mesh->vertexNormals, (int)mesh->texOffsetS,
                            (int)mesh->texOffsetT, d0, d1, mat->texS, mat->texT, q1, q2, fx1, fx2,
                            6);
    } else {
        int n = (d1 * d0) << 1;
        if (n > 0) {
            int i = 0;
            int off = 0;
            tw = n;
            do {
                s16 *p;
                p = (s16 *)((char *)mesh->faceNormals + off);
                if ((*p) > 0xfff) {
                    *p = 0xfff;
                }
                p = (s16 *)((char *)mesh->faceNormals + off);
                if (p[1] > 0xfff) {
                    p[1] = 0xfff;
                }
                p = (s16 *)((char *)mesh->faceNormals + off);
                if (p[2] > 0xfff) {
                    p[2] = 0xfff;
                }
                off += 6;
                i++;
            } while (i < tw);
        }
        func_ov007_020ca86c((int)mesh->verts, (int)mesh->faceNormals, d0, d1, 6);
    }
    *((volatile u32 *)0x4000448) = 2;
}
}
