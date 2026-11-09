//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "cstd.h"
#include "Ov007Mesh.h"
#include "Ov007Texture.h"

#define reg_G3_MTX_PUSH     (*(volatile u32 *)0x04000444)
#define reg_G3_MTX_POP      (*(volatile u32 *)0x04000448)
#define reg_G3_MTX_SCALE    (*(volatile u32 *)0x0400046c)
#define reg_G3_MTX_TRANS    (*(volatile u32 *)0x04000470)
#define reg_G3_POLYGON_ATTR (*(volatile u32 *)0x040004a4)
#define reg_G3_DIF_AMB      (*(volatile u32 *)0x040004c0)
#define reg_G3_SPE_EMI      (*(volatile u32 *)0x040004c4)

/* NitroSDK G3 immediate-mode commands (g3imm.h); the right-to-left argument
 * evaluation is the z, y, x load order the ROM shows for the translate.
 * File-local because adding them to include/nitro/gx/g3imm.h renumbers
 * ov075/d_s_entry_entry's pinned @NNN. */
static inline void G3_PushMtx(void) {
    reg_G3_MTX_PUSH = 0;
}
static inline void G3_PopMtx(int num) {
    reg_G3_MTX_POP = num;
}
static inline void G3_Translate(fx32 x, fx32 y, fx32 z) {
    reg_G3_MTX_TRANS = x;
    reg_G3_MTX_TRANS = y;
    reg_G3_MTX_TRANS = z;
}
static inline void G3_Scale(fx32 x, fx32 y, fx32 z) {
    reg_G3_MTX_SCALE = x;
    reg_G3_MTX_SCALE = y;
    reg_G3_MTX_SCALE = z;
}

extern "C" {

extern void func_ov007_020c1448(Ov007Texture *self, u32 texGen, u32 repeat, u32 flip, u32 color0);
extern void Texture_BindPalette(Ov007Texture *tex);
extern void func_ov007_020caeac(Vector3 *vtx, Vector3_16 *nrm, s32 *tu, s32 *tv, s32 w, s32 h,
                                s32 u0, s32 v0, s32 du, s32 dv, s32 su, s32 sv, s32 shift);
extern void func_ov007_020ca86c(Vector3 *vtx, Vector3_16 *nrm, s32 w, s32 h, s32 shift);

/* Draws the mesh (data_ov007_02104bd4[0]) with one of its materials
 * ([1] or [2]): sets the material's light colours, texture and polygon
 * attributes, translates by pos and scales z by scaleZ, then emits the
 * grid. With smooth set it is drawn textured with the per-point normals
 * (func_ov007_020caeac), otherwise flat with the face normals clamped to
 * 0xfff (func_ov007_020ca86c). */
void func_ov007_020ca5f0(Ov007Mesh *mesh, Ov007MeshMaterial *mat, int *pos, int scaleZ,
                         int smooth) {
    Ov007Texture *tex = mat->texture;
    Ov007Grid *grid = mesh->grid;
    int cellCols = grid->columns - 1;
    int cellRows = grid->rows - 1;
    int tw = 0;
    int th = 0;
    reg_G3_DIF_AMB = (u32)mat->diffuse | ((u32)mat->ambient << 16) | 0x8000;
    reg_G3_SPE_EMI = 0;
    if (tex != 0) {
        tw = 8 << tex->sizeS;
        th = 8 << tex->sizeT;
    }
    func_ov007_020c1448(tex, 1, mat->texRepeat, 0, mat->texColor0);
    Texture_BindPalette(tex);
    reg_G3_POLYGON_ATTR =
        0x8c1 | (mat->polygonMode << 4) | (mat->polygonId << 24) | (mat->alpha << 16);
    G3_PushMtx();
    G3_Translate(pos[0], pos[1], pos[2]);
    G3_Scale(0x1000, 0x1000, scaleZ);
    G3_PushMtx();
    G3_Scale(0x40000, 0x40000, 0x40000);
    if (smooth != 0) {
        int du = cstd::div(tw * mat->tilesS, cellCols);
        int dv = cstd::div(th * mat->tilesT, cellRows);
        int shiftS = FX_MUL(mesh->cellWidth, mat->warpS);
        int shiftT = FX_MUL(mesh->cellHeight, mat->warpT);
        func_ov007_020caeac(mesh->verts, mesh->vertexNormals, mesh->texOffsetS, mesh->texOffsetT,
                            cellCols, cellRows, mat->texS, mat->texT, du, dv, shiftS, shiftT, 6);
    } else {
        int i;
        for (i = 0; i < cellRows * cellCols * 2; i++) {
            if (mesh->faceNormals[i].x > 0xfff) {
                mesh->faceNormals[i].x = 0xfff;
            }
            if (mesh->faceNormals[i].y > 0xfff) {
                mesh->faceNormals[i].y = 0xfff;
            }
            if (mesh->faceNormals[i].z > 0xfff) {
                mesh->faceNormals[i].z = 0xfff;
            }
        }
        func_ov007_020ca86c(mesh->verts, mesh->faceNormals, cellCols, cellRows, 6);
    }
    G3_PopMtx(2);
}
}
